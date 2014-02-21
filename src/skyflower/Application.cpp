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

// Evil global variable until we have some kind of configuration system.
static bool g_quakeSounds = false;

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

	// Create graphics engine
	m_graphicsEngine = CreateGraphicsEngine();
	m_graphicsEngine->Init(m_window->GetHandle(), m_window->GetWidth(), m_window->GetHeight(), "../../content/");

	// FIXME: Tweaka dessa när vi har en riktig bana i spelet.
	m_SSAOradius = 0.7f;
	m_SSAOprojectionFactor = 0.3f;
	m_SSAObias = 0.15f;
	m_SSAOcontrast = 3.0f;
	m_SSAOsigma = 2.0f;
	m_graphicsEngine->SetSSAOParameters(m_SSAOradius, m_SSAOprojectionFactor, m_SSAObias, m_SSAOcontrast, m_SSAOsigma);
	m_graphicsEngine->SetDepthOfFieldFocusPlanes(0.0f, 0.0f, 300.0f, 400.0f);

	m_GUI = new GUI(m_graphicsEngine);

	m_soundEngine = CreateSoundEngine("../../content/sounds/");
	assert(m_soundEngine);

	m_menu = new Menu();
	m_menu->init(m_GUI, m_window->GetWidth(), m_window->GetHeight(), m_soundEngine);
	m_menu->setActive(false);

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
	modules.gui = m_GUI;
	
	m_backgroundMusicMenu.push_back("music/ants.opus");

	m_backgroundMusicGame.push_back("music/happy_piano.opus");

	m_backgroundMusicGameHubworld.push_back("music/happy_piano.opus");

	m_backgroundMusic = m_soundEngine->CreateSource();
	m_backgroundMusic->SetRelativeToListener(true);
	m_backgroundMusic->SetPlaybackFinishedHandler([this]() {
		m_backgroundMusicIndex = (m_backgroundMusicIndex + 1) % m_backgroundMusicList->size();

		m_backgroundMusic->SetResource(m_backgroundMusicList->at(m_backgroundMusicIndex));
		m_backgroundMusic->Play();
	});

	// Start playing some background music for the menu.
	m_backgroundMusic->SetVolume(0.25f);
	setBackgroundMusicList(m_backgroundMusicMenu);

	m_entityManager = new EntityManager("../../XML/", &modules);
	//m_entityManager->loadXML("player.xml");

	levelHandler->init(m_entityManager);

	// Load Hub Level
	levelHandler->queue(0);
	levelHandler->loadQueued();

	//m_entityManager->sendMessageToEntity("ActivateListener", "player");
	m_graphicsEngine->UpdateSceneData();
	m_entityManager->loadXML("subWorld1Lights.XML");
	
	m_showCharts = false;
	double chartDrawFrequency = 0.1;
	double nextChartDraw = 0.0;

	double chartTime = 20.0;
	double chartResolution = 1.0 / 30.0;
	// Make the charts hold 2 minutes worth of values at 60fps.
	size_t chartCapacity = 120 * 60;

	std::vector<const struct LineChartDataPoint> lineChartDataPoints;
	lineChartDataPoints.reserve((size_t)((chartTime / chartResolution) * 2));

	LineChartData frameTimeChart(chartCapacity);
	LineChartData fpsChart(chartCapacity);
	LineChartData memoryChart(chartCapacity);

	m_frameChartID =  m_GUI->CreateGUIElementAndBindTexture(Vec3(0.0f, 0.0f, 0.0f), m_GUI->CreateTexture2D(256, 128, true));
	m_fpsChartID =  m_GUI->CreateGUIElementAndBindTexture(Vec3(0.0f, 128.0f + 6.0f, 0.0f), m_GUI->CreateTexture2D(256, 128, true));
	m_memChartID =  m_GUI->CreateGUIElementAndBindTexture(Vec3(0.0f, 2 * (128.0f + 6.0f), 0.0f), m_GUI->CreateTexture2D(256, 128, true));

	LineChartRendererD3D lineChartRenderer(m_graphicsEngine);

	// Don't start listening to input events until everything has been initialized.
	m_inputHandler->AddListener(this);

	double time, deltaTime;

	//startTime = GetTime();

	m_cutscene = new CutScene(m_entityManager->modules->script, m_camera);

	int loadingScreen = m_GUI->CreateGUIElementAndBindTexture(Vec3::Zero(), "Menygrafik\\fyraTreRatio.png");
	m_GUI->GetGUIElement(loadingScreen)->SetVisible(false);

	m_menuCameraRotation = 0.0f;

	mGameTime = 0.0;
	m_oldTime = GetTime();
	mStartTime = GetTime();
	m_quit = false;

	while(!m_quit)
	{
		time = GetTime();
		deltaTime = time - m_oldTime;
		m_oldTime = time;

		mGameTime = time - mStartTime;

		frameTimeChart.AddDataPoint(time, deltaTime * 1000.0);
		fpsChart.AddDataPoint(time, 1.0 / deltaTime);
		memoryChart.AddDataPoint(time, GetMemoryUsage() / (1024.0 * 1024.0));

		if (m_showCharts && time >= nextChartDraw) {
			nextChartDraw = time + chartDrawFrequency;

			// Draw the frame time chart.
			lineChartDataPoints.clear();
			frameTimeChart.PushDataPoints(time - chartTime, time, chartResolution, lineChartDataPoints);

			lineChartRenderer.SetTargetValue((1.0 / 60.0) * 1000.0);
			lineChartRenderer.SetUnit("ms");
			lineChartRenderer.SetLabel("frame time");
			lineChartRenderer.Draw(lineChartDataPoints, time - chartTime, m_GUI->GetGUIElement(m_frameChartID)->GetTexture());

			// Draw the FPS chart.
			lineChartDataPoints.clear();
			fpsChart.PushDataPoints(time - chartTime, time, chartResolution, lineChartDataPoints);

			lineChartRenderer.SetTargetValue(60.0);
			lineChartRenderer.SetUnit("fps");
			lineChartRenderer.SetLabel("FPS");
			lineChartRenderer.Draw(lineChartDataPoints, time - chartTime, m_GUI->GetGUIElement(m_fpsChartID)->GetTexture());

			// Draw the RAM chart.
			lineChartDataPoints.clear();
			memoryChart.PushDataPoints(time - chartTime, time, chartResolution, lineChartDataPoints);

			lineChartRenderer.SetTargetValue(256.0);
			lineChartRenderer.SetUnit("MiB");
			lineChartRenderer.SetLabel("RAM");
			lineChartRenderer.Draw(lineChartDataPoints, time - chartTime, m_GUI->GetGUIElement(m_memChartID)->GetTexture());
		}

		float volume = m_menu->getSettings()._soundVolume;

		if (m_oldVolume != volume)
		{
			ListenerComponent* playerListener = (ListenerComponent*)m_entityManager->getComponent("player", "Listener");
			playerListener->setVolume(volume);
			m_oldVolume = volume;
		}
		switch (gameState)
		{
		case GameState::game:
			updateGame((float)deltaTime, (float)mGameTime);
			break;
		case GameState::loading:
			updateLoading((float)deltaTime);
			break;
		case GameState::menu:
			updateMenu((float)deltaTime, (float)mGameTime);
			break;
		case GameState::cutScene:
			updateCutScene((float)deltaTime);
			break;
		}
		
		m_GUI->Draw();

		m_graphicsEngine->Present();

		m_physicsEngine->Update((float)deltaTime);

		m_soundEngine->Update((float)deltaTime);
		m_window->PumpMessages();

		if (levelHandler->hasQueuedLevel() && !levelHandler->isLoading())
		{
			// Basically a hax - Dont do this at home
			// FIXME: Don't hardcode this!
			m_GUI->GetGUIElement(loadingScreen)->GetDrawInput()->scale = XMFLOAT2((float)m_window->GetWidth() / 1024, (float)m_window->GetHeight() / 768);
			m_GUI->GetGUIElement(loadingScreen)->SetVisible(true);
			m_GUI->Draw();
			m_graphicsEngine->Present();

			levelHandler->loadQueued();
			m_graphicsEngine->Clear();
			m_graphicsEngine->UpdateSceneData();

			m_GUI->GetGUIElement(loadingScreen)->SetVisible(false);

			if (levelHandler->currentLevel() == 0)
				setBackgroundMusicList(m_backgroundMusicGameHubworld);
			else
				setBackgroundMusicList(m_backgroundMusicGame);

			m_oldTime = GetTime();
		}
	}
	
	//m_graphicsEngine->DeleteTexture2D(memoryChartTexture);
	//m_graphicsEngine->DeleteTexture2D(frameTimeChartTexture);

	delete m_cutscene;
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

void Application::updateMenu(float dt, float gameTime)
{
	// If m has been pressed again
	if (!m_menu->isActive())
	{
		changeGameState(GameState::game);
		return;
	}

	if (m_inputHandler->isMouseButtonDown(MouseButton::MouseButtonLeft))
	{
		int x, y;
		m_inputHandler->GetMousePosition(x, y);
		m_menu->onMouseDown(Vec3(x, y));
	}

	if (m_menu->getSettings()._isFullscreen && !m_graphicsEngine->IsFullscreen())
	{
		m_graphicsEngine->SetFullscreen(true);
		this->OnWindowResized(1920, 1080);
		m_oldTime = GetTime();
	}
	else if (!m_menu->getSettings()._isFullscreen && m_graphicsEngine->IsFullscreen())
	{
		m_graphicsEngine->SetFullscreen(false);
		this->OnWindowResized(1024, 768);
		m_oldTime = GetTime();
	}

	if (m_camera->GetMouseSense() != m_menu->getSettings()._mouseSense)
		m_camera->SetMouseSense(m_menu->getSettings()._mouseSense);

	float cameraDistance = 100.0f;
	Vec3 followTarget = m_entityManager->getEntityPos("player");

	m_menuCameraRotation += DegreesToRadians(15.0f) * dt;

	m_camera->Follow(followTarget);
	m_camera->SetPosition(followTarget + Vec3(cosf(m_menuCameraRotation) * cameraDistance, 150.0f, sinf(m_menuCameraRotation) * cameraDistance));
	m_camera->Update(dt);

	m_graphicsEngine->UpdateScene(dt, gameTime);
	m_graphicsEngine->DrawScene();

	m_menu->draw();

	switch (m_menu->getStatus())
	{
	case Menu::resume:

		m_menu->setActive(false);

		// Set if mouse is inverted based on if it's been checked in menu
		m_camera->SetInverted(m_menu->getSettings()._mouseInverted);
		if (m_cutscene->isPlaying())
			changeGameState(GameState::cutScene);
		else
			changeGameState(GameState::game);
		break;
	case Menu::exit:
		m_quit = true;
	}
}

void Application::updateCutScene(float dt)
{
	m_entityManager->update(dt);
	m_camera->Update(dt);
	m_graphicsEngine->UpdateScene(dt, (float)mGameTime);
	m_graphicsEngine->DrawScene();

	m_cutscene->update(dt);

	if (m_menu->isActive())
		changeGameState(GameState::menu);
	if (!m_cutscene->isPlaying())
	{
		changeGameState(GameState::game);
	}
}

void Application::updateGame(float dt, float gameTime)
{
	Vec3 playerPos = m_entityManager->getEntityPos("player");
	m_camera->Follow(playerPos);

	Movement* playerMove = m_entityManager->getEntity(1)->getComponent<Movement*>("Movement");
	playerMove->setCamera(m_camera->GetLook(), m_camera->GetRight(), m_camera->GetUp());
	playerMove->setYaw(m_camera->GetYaw());


	//camera collision
	Ray ray = Ray(playerPos + Vec3(0, 10, 0), (playerPos + Vec3(0, 10, 0) - m_camera->GetPosition()).Normalize()*-100);
	//std::vector<CollisionInstance*> cols = m_collision->GetCollisionInstances();
	for (int i = 0; i < m_entityManager->getNrOfEntities(); i++)
	{
		if (m_entityManager->getEntityByIndex(i)->getType() == "plattform")
		{
			float t = m_entityManager->getEntityByIndex(i)->collInst->Test(ray);
			if (t > 0)
			{
				Vec3 dir = ray.GetDir();
				ray.Set(ray.GetPos(), dir*t);
				m_camera->SetOffsetFast(ray.GetDir().Length() - 5);
			}
		}
	}

	m_camera->Rotate(m_camera->GetYaw(), m_camera->GetPitch());
	m_camera->Update(dt);

	m_entityManager->update(dt);

	m_graphicsEngine->UpdateScene(dt, gameTime);
	m_graphicsEngine->DrawScene();

	if (m_cutscene->isPlaying())
		changeGameState(GameState::cutScene);

	if (m_menu->isActive())
		changeGameState(GameState::menu);
}

void Application::updateLoading(float dt)
{
	Draw2DInput input;
	input.pos.x = 0.0f;
	input.pos.y = 0.0f;

	if (!levelHandler->isLoading())
		changeGameState(GameState::game);
}

void Application::changeGameState(GameState newState)
{
	switch (newState)
	{
	case GameState::game:
		m_backgroundMusic->SetVolume(0.01f);

		if (levelHandler->currentLevel() == 0)
			setBackgroundMusicList(m_backgroundMusicGameHubworld);
		else
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
			m_entityManager->sendGlobalMessage("MenuActivated");
			m_inputHandler->SetMouseCapture(false);
			m_window->SetCursorVisibility(true);
		}
		else
		{
			m_entityManager->sendGlobalMessage("MenuDeactivated");
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
	m_oldTime = GetTime();
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
		m_oldTime = GetTime();
	}
}

void Application::OnWindowDeactivate()
{
	m_window->SetCursorVisibility(true);
}

void Application::OnMouseMove(int deltaX, int deltaY)
{
	if (m_camera && gameState == GameState::game)
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
		if (m_menu->isActive())
			m_menu->setActive(false);
	
		else
			m_menu->setActive(true);
		break;
	case 'Z':
		m_showCharts = !m_showCharts;

		m_GUI->GetGUIElement(m_frameChartID)->SetVisible(m_showCharts);
		m_GUI->GetGUIElement(m_fpsChartID)->SetVisible(m_showCharts);
		m_GUI->GetGUIElement(m_memChartID)->SetVisible(m_showCharts);
		break;
	case 'N':
		g_quakeSounds = !g_quakeSounds;
		break;
	case 'R':
		m_graphicsEngine->ClearLights();
		levelHandler->queue(5);
		//m_entityManager->loadXML("subWorld2Lights.XML");
			
		break;
	case VK_SPACE:
		if (m_cutscene->isPlaying())
			m_cutscene->stop();
	case 'P':
		m_graphicsEngine->SetPostProcessingEffects(m_graphicsEngine->GetPostProcessingEffects() ^ POST_PROCESSING_SSAO);
		break;
	case 'O':
		m_graphicsEngine->SetPostProcessingEffects(m_graphicsEngine->GetPostProcessingEffects() ^ POST_PROCESSING_DOF);
		break;
#if 0
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
#endif
	default:
		break;
	}
}

std::string GetPlayerSoundFile(const std::string &file)
{
	if (!g_quakeSounds)
		return file;

	size_t pos = file.find_last_of('/');
	std::string result;

	if (pos != std::string::npos) {
		result = file.substr(0, pos) + "/quake" + file.substr(pos, std::string::npos);
	}
	else {
		result = file;
	}

	return result;
}
