#include <string>
#include <iostream>
#include <cassert>

#include "shared/util.h"
#include "Sound/SoundEngine.h"

#include "Application.h"
#include "tinyxml2.h"
#include "ComponentHeaders.h"
#include "LineChart.h"

// Must be included last!
#include "shared/debug.h"

using namespace std;
using namespace tinyxml2;
using namespace Cistron;

Application::Application()
{
	m_oldVolume = 1.0f;
}

Application::~Application()
{
}

void Application::Start()
{
	m_window = new Window(1024, 768, L"Skyflower");
	m_window->SetListener(this);

	m_inputHandler = m_window->GetInputHandler();
	m_inputHandler->AddListener(this);

	// Create graphics engine
	m_graphicsEngine = CreateGraphicsEngine();
	m_graphicsEngine->Init(m_window->GetHandle(), m_window->GetWidth(), m_window->GetHeight(), "../../content/");

	// FIXME: Tweaka dessa n�r vi har en riktig bana i spelet.
	m_SSAOradius = 0.7f;
	m_SSAOprojectionFactor = 0.3f;
	m_SSAObias = 0.15f;
	m_SSAOcontrast = 3.0f;
	m_SSAOsigma = 2.0f;
	m_graphicsEngine->SetSSAOParameters(m_SSAOradius, m_SSAOprojectionFactor, m_SSAObias, m_SSAOcontrast, m_SSAOsigma);
	m_graphicsEngine->SetDepthOfFieldFocusPlanes(0.0f, 0.0f, 300.0f, 400.0f);

	m_GUI = new GUI(m_graphicsEngine);

	m_menu = new Menu();
	m_menu->init(m_GUI, m_window->GetWidth(), m_window->GetHeight());
	m_menu->setActive(false);

	m_soundEngine = CreateSoundEngine("../../content/sounds/");
	assert(m_soundEngine);

	m_camera = m_graphicsEngine->CreateCameraController();
	m_physicsEngine = CreatePhysicsEngine();
	m_collision = CreateCollision("../../content/");
	m_potentialField = new PotentialField();
	m_scriptHandler = new ScriptHandler();
	Event::Register(m_scriptHandler);

	Modules modules;
	modules.input = m_inputHandler;
	modules.graphics = m_graphicsEngine;
	modules.sound = m_soundEngine;
	modules.camera = m_camera;
	modules.physicsEngine = m_physicsEngine;
	modules.collision = m_collision;
	modules.potentialField = m_potentialField;
	modules.script = m_scriptHandler;
	
	m_backgroundMusicMenu.push_back("music/ants.opus");

	m_backgroundMusicGame.push_back("music/creepy.opus");
	//m_backgroundMusicGame.push_back("music/wat.opus");

	m_backgroundMusic = m_soundEngine->CreateSource();
	m_backgroundMusic->SetRelativeToListener(true);
	m_backgroundMusic->SetPlaybackFinishedHandler([this]() {
		m_backgroundMusicIndex = (m_backgroundMusicIndex + 1) % m_backgroundMusicList->size();

		m_backgroundMusic->SetResource(m_backgroundMusicList->at(m_backgroundMusicIndex));
		m_backgroundMusic->Play();
	});

	// Start playing some background music for the menu.
	m_backgroundMusic->SetVolume(0.05f);
	setBackgroundMusicList(m_backgroundMusicMenu);

	m_entityManager = new EntityManager("../../XML/", &modules);
	m_entityManager->loadXML("player.xml");

	levelHandler->init(m_entityManager);

	// Load Hub Level
	levelHandler->queue(3);
	levelHandler->LoadQueued();
	m_entityManager->sendMessageToEntity("ActivateListener", "player");
	m_graphicsEngine->UpdateSceneData();
	m_entityManager->loadXML("subWorld1Lights.XML");
	Movement* playerMove = (Movement*)m_entityManager->getComponent("player", "Movement");
	ListenerComponent* playerListener = (ListenerComponent*)m_entityManager->getComponent("player", "Listener");

	m_showCharts = false;

	// Make the charts hold 60 seconds worth of values at 60fps.
	size_t chartCapacity = 60 * 60;
	LineChart frameTimeChart(chartCapacity);
	frameTimeChart.SetSize(256, 128);
	frameTimeChart.SetUnit("ms");
	//Texture2D *frameTimeChartTexture = m_graphicsEngine->CreateTexture2D(frameTimeChart.GetWidth(), frameTimeChart.GetHeight());
	m_frameChartID =  m_GUI->CreateGUIElementAndBindTexture(Vec3(0.0f, 0.0f, 0.0f), 
		m_GUI->CreateTexture2D(frameTimeChart.GetWidth(), frameTimeChart.GetHeight()));

	LineChart memoryChart(chartCapacity);
	memoryChart.SetSize(256, 128);
	memoryChart.SetUnit("MiB");
	//Texture2D *memoryChartTexture = m_graphicsEngine->CreateTexture2D(memoryChart.GetWidth(), memoryChart.GetHeight());
	m_memChartID =  m_GUI->CreateGUIElementAndBindTexture(Vec3(0.0f, (float)(frameTimeChart.GetHeight() + 6), 0.0f),
		m_GUI->CreateTexture2D(memoryChart.GetWidth(), memoryChart.GetHeight()));

	thread load;

	double chartUpdateDelay = 0.1;
	double nextChartUpdate = 0.0;
	double chartTime = 30.0;

	double time, deltaTime;

	//startTime = GetTime();

	mGameTime = 0.0;
	m_oldTime = GetTime();
	mStartTime = GetTime();
	m_quit = false;

	changeGameState(GameState::cutScene);
	cs = new CutScene(m_entityManager->modules->script, m_camera);
	cs->play();

	while(!m_quit)
	{
		time = GetTime();
		deltaTime = time - m_oldTime;
		m_oldTime = time;

		mGameTime = time - mStartTime;

		frameTimeChart.AddPoint(time, deltaTime * 1000.0);
		memoryChart.AddPoint(time, GetMemoryUsage() / (1024.0 * 1024.0));

		if (m_showCharts && time >= nextChartUpdate) {
			nextChartUpdate = time + chartUpdateDelay;

			frameTimeChart.Draw(time - chartTime, time, 1.0 / 60.0, (1.0 / 60.0) * 1000.0);
			m_GUI->UploadData(m_frameChartID, frameTimeChart.GetPixels());

			memoryChart.Draw(time - chartTime, time, 1.0 / 100.0, 256.0);
			m_GUI->UploadData(m_memChartID, memoryChart.GetPixels());
		}

		if (levelHandler->hasQueuedLevel() && !levelHandler->isLoading())
		{
			if (load.joinable())
				load.join();

			load = thread(&LevelHandler::LoadQueued, levelHandler);
			changeGameState(GameState::loading);
		}

		float volume = m_menu->getSettings()._soundVolume;

		if (m_oldVolume != volume)
		{
			playerListener->setVolume(volume);
			m_oldVolume = volume;
		}

		switch (gameState)
		{
		case GameState::game:
			updateGame((float)deltaTime, (float)mGameTime, playerMove);
			break;
		case GameState::loading:
			updateLoading((float)deltaTime);
			break;
		case GameState::menu:
			updateMenu((float)deltaTime);
			break;
		case GameState::cutScene:
			updateCutScene((float)deltaTime);
			break;
		}
		
		m_GUI->Draw();

		m_graphicsEngine->Present();

		m_soundEngine->Update((float)deltaTime);
		m_window->PumpMessages();
	}
	
	//m_graphicsEngine->DeleteTexture2D(memoryChartTexture);
	//m_graphicsEngine->DeleteTexture2D(frameTimeChartTexture);

	delete cs;
	delete m_menu;
	m_GUI->Destroy();
	delete m_GUI;
	delete levelHandler;
	delete m_entityManager;
	delete m_scriptHandler;
	delete m_potentialField;
	DestroyCollision(m_collision);
	DestroyPhysicsEngine(m_physicsEngine);
	m_soundEngine->DestroySource(m_backgroundMusic);
	DestroySoundEngine(m_soundEngine);
	m_graphicsEngine->DeleteCameraController(m_camera);
	DestroyGraphicsEngine(m_graphicsEngine);
	delete m_window;
}

void Application::updateMenu(float dt)
{
	// If m has been pressed again
	if (!m_menu->isActive())
		changeGameState(GameState::game);

	m_menu->draw();

	switch (m_menu->getStatus())
	{
	case Menu::resume:
		if (m_menu->getSettings()._isFullscreen && !m_graphicsEngine->isFullscreen())
		{
			m_graphicsEngine->SetFullscreen(true);
			this->OnWindowResized(1920, 1080);
		}
		else if (!m_menu->getSettings()._isFullscreen && m_graphicsEngine->isFullscreen())
		{
			m_graphicsEngine->SetFullscreen(false);
			this->OnWindowResized(1024, 768);
		}

		m_menu->setActive(false);

		// Set if mouse is inverted based on if it's been checked in menu
		m_camera->SetInverted(m_menu->getSettings()._mouseInverted);
		changeGameState(GameState::game);
		break;
	case Menu::exit:
		m_quit = true;
	}
}

void Application::updateCutScene(float dt)
{
	m_camera->Update(dt);
	m_graphicsEngine->UpdateScene(dt, (float)mGameTime);
	m_graphicsEngine->DrawScene();

	cs->update(dt);

	if (m_menu->isActive())
		changeGameState(GameState::menu);
	if (cs->isDone())
	{
		changeGameState(GameState::game);
	}
}

void Application::updateGame(float dt, float gameTime, Movement* playerMove)
{
	m_camera->Follow(m_entityManager->getEntityPos("player"));
	playerMove->setCamera(m_camera->GetLook(), m_camera->GetRight(), m_camera->GetUp());
	playerMove->setYaw(m_camera->GetYaw());
	m_camera->Update(dt);
	
	m_graphicsEngine->UpdateScene(dt, gameTime);
	m_graphicsEngine->DrawScene();
	m_entityManager->update(dt);

	if (m_menu->isActive())
		changeGameState(GameState::menu);
}

void Application::updateLoading(float dt)
{
	Draw2DInput input;
	input.pos.x = 0.0f;
	input.pos.y = 0.0f;

	m_graphicsEngine->Begin2D();
	m_graphicsEngine->Draw2DTextureFile("..\\..\\content\\Textures\\Menygrafik\\fyraTreRatio.png", &input);
	m_graphicsEngine->End2D();

	if (!levelHandler->isLoading())
		changeGameState(GameState::game);
}

void Application::changeGameState(GameState newState)
{
	switch (newState)
	{
	case GameState::game:
		m_backgroundMusic->SetVolume(0.01f);
		setBackgroundMusicList(m_backgroundMusicGame);
		break;
	case GameState::menu:
		m_backgroundMusic->SetVolume(0.05f);
		setBackgroundMusicList(m_backgroundMusicMenu);
		break;
	default:
		break;
	}

	if (m_window->IsActive())
	{
		if (newState == GameState::menu)
		{
			m_inputHandler->SetMouseCapture(false);
			m_window->SetCursorVisibility(true);
		}
		else
		{
			m_inputHandler->SetMouseCapture(true);
			m_window->SetCursorVisibility(false);
		}
	}

	gameState = newState;
}

void Application::setBackgroundMusicList(const std::vector<std::string> &musicList)
{
	if (m_backgroundMusicList == &musicList)
		return;

	m_backgroundMusicIndex = 0;
	m_backgroundMusicList = &musicList;

	m_backgroundMusic->SetResource(musicList[0]);
	m_backgroundMusic->Play();
}

void Application::OnWindowShouldClose()
{
	m_quit = true;
}

void Application::OnWindowResized(unsigned int width, unsigned int height)
{
	m_graphicsEngine->OnResize(width, height);
	m_menu->onResize(width, height);
}

void Application::OnWindowResizeEnd()
{
	// While resizing/moving the window PumpMessages will block and cause a
	// huge delta time when the window has stopped resizing/moving. Set
	// m_oldTime to the current time to prevent that from happening.
	m_oldTime = GetTime();
}

void Application::OnWindowActivate()
{
	if (gameState == GameState::game)
	{
		m_inputHandler->SetMouseCapture(true);
		m_window->SetCursorVisibility(false);
	}
}

void Application::OnWindowDeactivate()
{
	m_window->SetCursorVisibility(true);
}

void Application::OnMouseMove(int deltaX, int deltaY)
{
	if (m_camera)
		m_camera->onMouseMove((float)deltaX, (float)deltaY);
	if (m_menu->isActive())
	{
		int x, y;
		m_inputHandler->GetMousePosition(x, y);
		m_menu->onMouseMove(Vec3(x, y));
	}
		
}

void Application::OnMouseButtonDown(enum MouseButton button)
{
	if (gameState == GameState::game && !m_inputHandler->IsMouseCaptured()) {
		m_inputHandler->SetMouseCapture(true);
		m_window->SetCursorVisibility(false);
	}
	if (gameState == GameState::menu && m_menu->isActive())
	{
		int x, y;
		m_inputHandler->GetMousePosition(x, y);
		m_menu->mousePressed(Vec3(x, y));
	}
}

void Application::OnMouseButtonUp(enum MouseButton button)
{
}

void Application::OnMouseWheel(int delta)
{
	m_camera->Zoom((float)delta, 8.0f);
}

void Application::OnKeyUp(unsigned short key)
{
}

void Application::OnKeyDown(unsigned short key)
{
	if (m_menu->isActive())
		m_menu->keyPressed(key);

	switch (key)
	{
	case VK_ESCAPE:
		m_inputHandler->SetMouseCapture(false);
		m_window->SetCursorVisibility(true);
		break;
	case 'Z':
		m_showCharts = !m_showCharts;

		m_GUI->GetGUIElement(m_frameChartID)->SetVisible(m_showCharts);
		m_GUI->GetGUIElement(m_memChartID)->SetVisible(m_showCharts);
		break;
	case 'R':
		m_graphicsEngine->clearLights();
		m_entityManager->loadXML("subWorld1Lights.XML");
		break;
	case 'M':
		if (m_menu->isActive())
			m_menu->setActive(false);
		else
		{
			m_graphicsEngine->Clear();
			m_menu->setActive(true);
		}
		break;
	case VK_SPACE:
		if (!cs->isDone())
			cs->quit(); 
	case 'P':
		m_graphicsEngine->SetPostProcessingEffects(m_graphicsEngine->GetPostProcessingEffects() ^ POST_PROCESSING_SSAO);
		break;
	case 'O':
		m_graphicsEngine->SetPostProcessingEffects(m_graphicsEngine->GetPostProcessingEffects() ^ POST_PROCESSING_DOF);
		break;
	case 'T':
	{
		static const size_t num_taunts = 2;
		static const char *taunts[num_taunts] = {
			"quake/taunt1.wav",
			"quake/taunt2.wav"
		};

		Vec3 position = Vec3(0.0f, 0.0f, 0.0f);
		m_soundEngine->PlaySound(taunts[rand() % num_taunts], &position.X, 0.25f, true);
		break;
	}
	case 'Y':
		m_SSAOradius += 0.1f;
		m_graphicsEngine->SetSSAOParameters(m_SSAOradius, m_SSAOprojectionFactor, m_SSAObias, m_SSAOcontrast, m_SSAOsigma);

		printf("radius=%.1f, projection factor=%.1f, bias=%.2f, contrast=%.1f, sigma=%.1f\n", m_SSAOradius, m_SSAOprojectionFactor, m_SSAObias, m_SSAOcontrast, m_SSAOsigma);
		break;
	case 'H':
		m_SSAOradius -= 0.1f;
		m_graphicsEngine->SetSSAOParameters(m_SSAOradius, m_SSAOprojectionFactor, m_SSAObias, m_SSAOcontrast, m_SSAOsigma);

		printf("radius=%.1f, projection factor=%.1f, bias=%.2f, contrast=%.1f, sigma=%.1f\n", m_SSAOradius, m_SSAOprojectionFactor, m_SSAObias, m_SSAOcontrast, m_SSAOsigma);
		break;
	case 'U':
		m_SSAOprojectionFactor += 0.1f;
		m_graphicsEngine->SetSSAOParameters(m_SSAOradius, m_SSAOprojectionFactor, m_SSAObias, m_SSAOcontrast, m_SSAOsigma);

		printf("radius=%.1f, projection factor=%.1f, bias=%.2f, contrast=%.1f, sigma=%.1f\n", m_SSAOradius, m_SSAOprojectionFactor, m_SSAObias, m_SSAOcontrast, m_SSAOsigma);
		break;
	case 'J':
		m_SSAOprojectionFactor -= 0.1f;
		m_graphicsEngine->SetSSAOParameters(m_SSAOradius, m_SSAOprojectionFactor, m_SSAObias, m_SSAOcontrast, m_SSAOsigma);

		printf("radius=%.1f, projection factor=%.1f, bias=%.2f, contrast=%.1f, sigma=%.1f\n", m_SSAOradius, m_SSAOprojectionFactor, m_SSAObias, m_SSAOcontrast, m_SSAOsigma);

		break;
	case 'I':
		m_SSAObias += 0.05f;
		m_graphicsEngine->SetSSAOParameters(m_SSAOradius, m_SSAOprojectionFactor, m_SSAObias, m_SSAOcontrast, m_SSAOsigma);

		printf("radius=%.1f, projection factor=%.1f, bias=%.2f, contrast=%.1f, sigma=%.1f\n", m_SSAOradius, m_SSAOprojectionFactor, m_SSAObias, m_SSAOcontrast, m_SSAOsigma);
		break;
	case 'K':
		m_SSAObias -= 0.05f;
		m_graphicsEngine->SetSSAOParameters(m_SSAOradius, m_SSAOprojectionFactor, m_SSAObias, m_SSAOcontrast, m_SSAOsigma);
		
		printf("radius=%.1f, projection factor=%.1f, bias=%.2f, contrast=%.1f, sigma=%.1f\n", m_SSAOradius, m_SSAOprojectionFactor, m_SSAObias, m_SSAOcontrast, m_SSAOsigma);
		break;
	case 'C':
		m_SSAOcontrast += 0.5f;
		m_graphicsEngine->SetSSAOParameters(m_SSAOradius, m_SSAOprojectionFactor, m_SSAObias, m_SSAOcontrast, m_SSAOsigma);

		printf("radius=%.1f, projection factor=%.1f, bias=%.2f, contrast=%.1f, sigma=%.1f\n", m_SSAOradius, m_SSAOprojectionFactor, m_SSAObias, m_SSAOcontrast, m_SSAOsigma);
		break;
	case 'V':
		m_SSAOcontrast -= 0.5f;
		m_graphicsEngine->SetSSAOParameters(m_SSAOradius, m_SSAOprojectionFactor, m_SSAObias, m_SSAOcontrast, m_SSAOsigma);

		printf("radius=%.1f, projection factor=%.1f, bias=%.2f, contrast=%.1f, sigma=%.1f\n", m_SSAOradius, m_SSAOprojectionFactor, m_SSAObias, m_SSAOcontrast, m_SSAOsigma);
		break;
	case 'B':
		m_SSAOsigma += 0.5f;
		m_graphicsEngine->SetSSAOParameters(m_SSAOradius, m_SSAOprojectionFactor, m_SSAObias, m_SSAOcontrast, m_SSAOsigma);

		printf("radius=%.1f, projection factor=%.1f, bias=%.2f, contrast=%.1f, sigma=%.1f\n", m_SSAOradius, m_SSAOprojectionFactor, m_SSAObias, m_SSAOcontrast, m_SSAOsigma);
		break;
	case 'N':
		m_SSAOsigma -= 0.5f;
		m_graphicsEngine->SetSSAOParameters(m_SSAOradius, m_SSAOprojectionFactor, m_SSAObias, m_SSAOcontrast, m_SSAOsigma);

		printf("radius=%.1f, projection factor=%.1f, bias=%.2f, contrast=%.1f, sigma=%.1f\n", m_SSAOradius, m_SSAOprojectionFactor, m_SSAObias, m_SSAOcontrast, m_SSAOsigma);
		break;
	default:
		break;
	}
}
