#include <string>
#include <iostream>
#include <cassert>

#include "shared/util.h"
#include "Sound/SoundEngine.h"

#include "Application.h"
#include "tinyxml2.h"
#include "ComponentHeaders.h"
#include "LineChart.h"

using namespace std;
using namespace tinyxml2;
using namespace Cistron;

Application::Application()
{
	m_window = NULL;
	m_soundEngine = NULL;
	this->entityManager = NULL;
	m_showCharts = false;
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
	camera = m_graphicsEngine->CreateCameraController();

	m_soundEngine = CreateSoundEngine("../../content/sounds/");
	assert(m_soundEngine);

	// Play some background music.
	//float soundPosition[3] = {0.0f, 0.0f, 0.0f};
	//m_soundEngine->PlaySound("creepy.opus", soundPosition, 0.05f, true);

	Modules modules;
	modules.input = m_inputHandler;
	modules.graphics = m_graphicsEngine;
	modules.sound = m_soundEngine;
	modules.potentialField = new PotentialField();
	
	entityManager = new EntityManager("../../XML/", &modules);

	entityManager->loadXML("player.xml");
	entityManager->loadXML("lights.XML");
	levelHandler->init(entityManager);

	// Load Hub Level
	levelHandler->queue(1);
	levelHandler->LoadQueued();
	entityManager->createSphereOnEntities();
	camera = m_graphicsEngine->CreateCameraController();
	m_graphicsEngine->UpdateSceneData();
	Movement* playerMove = (Movement*)entityManager->getComponent("player", "Movement");

	entityManager->sendMessageToEntity("ActivateListener", "player");

	LineChart frameTimeChart(1024 * 1024);
	frameTimeChart.SetSize(256, 128);
	frameTimeChart.SetUnit("ms");
	Texture2D *frameTimeChartTexture = m_graphicsEngine->CreateTexture2D(frameTimeChart.GetWidth(), frameTimeChart.GetHeight());

	LineChart memoryChart(1024 * 1024);
	memoryChart.SetSize(256, 128);
	memoryChart.SetUnit("MiB");
	Texture2D *memoryChartTexture = m_graphicsEngine->CreateTexture2D(memoryChart.GetWidth(), memoryChart.GetHeight());

	double chartUpdateDelay = 0.1;
	double nextChartUpdate = 0.0;
	double chartTime = 30.0;

	double oldTime = GetTime();
	double time, deltaTime;

	double timeSinceLight = 0;
	m_quit = false;

	thread load;
	m_menu = new Menu();
	m_menu->init(m_graphicsEngine);
	m_menu->setActive(false);

	while(!m_quit)
	{
		time = GetTime();
		deltaTime = time - oldTime;
		oldTime = time;
		timeSinceLight += deltaTime;

		frameTimeChart.AddPoint((float)time, (float)(deltaTime * 1000.0));
		memoryChart.AddPoint((float)time, GetMemoryUsage() / (1024.0f * 1024.0f));

		if (m_showCharts && time >= nextChartUpdate) {
			nextChartUpdate = time + chartUpdateDelay;

			frameTimeChart.Draw((float)(time - chartTime), (float)time, 1.0f / 100.0f, (1.0f / 60.0f) * 1000.0f);
			frameTimeChartTexture->UploadData(frameTimeChart.GetPixels());

			memoryChart.Draw((float)(time - chartTime), (float)time, 1.0f / 100.0f, 256.0f);
			memoryChartTexture->UploadData(memoryChart.GetPixels());
		}

		m_graphicsEngine->Begin2D();

		if (m_showCharts) {
			m_graphicsEngine->Draw2DTexture(frameTimeChartTexture, 0, 0);
			m_graphicsEngine->Draw2DTexture(memoryChartTexture, 0, frameTimeChart.GetHeight() + 6);
		}
		m_graphicsEngine->End2D();

		m_soundEngine->Update((float)deltaTime);
		m_window->PumpMessages();
		
		
		if (levelHandler->hasQueuedLevel() && !levelHandler->isLoading())
		{
			if (load.joinable())
				load.join();
			load = thread(&LevelHandler::LoadQueued, levelHandler);
			changeGameState(GameState::loading);
		}
			
		switch (gameState)
		{
		case GameState::game:
			updateGame((float)deltaTime, playerMove);
			break;
		case GameState::loading:
			updateLoading((float)deltaTime);
			break;
		case GameState::menu:
			updateMenu((float)deltaTime);
			break;
		}
		
		m_graphicsEngine->Present();
		
	}

	m_graphicsEngine->DeleteTexture2D(memoryChartTexture);
	m_graphicsEngine->DeleteTexture2D(frameTimeChartTexture);
	delete levelHandler;
	DestroyCameraController(camera);
	delete entityManager;
	DestroySoundEngine(m_soundEngine);
	DestroyGraphicsEngine(m_graphicsEngine);
	delete m_window;
}

void Application::OnWindowShouldClose()
{
	m_quit = true;
}

void Application::OnWindowResize(unsigned int width, unsigned int height)
{
	m_graphicsEngine->OnResize(width, height);
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
	if (camera)
		camera->RotateCamera((float)deltaX, (float)deltaY);
}

void Application::OnMouseButtonDown(enum MouseButton button)
{
	if (gameState == GameState::game && !m_inputHandler->IsMouseCaptured()) {
		m_inputHandler->SetMouseCapture(true);
		m_window->SetCursorVisibility(false);
	}
}

void Application::OnMouseButtonUp(enum MouseButton button)
{
}

void Application::OnMouseWheel(int delta)
{
	camera->Zoom((float)delta, 10.0f);
}

void Application::OnKeyDown(unsigned short key)
{
	if (m_menu->isActive())
		m_menu->buttonPressed(key);
	switch (key)
	{
	case VK_ESCAPE:
		m_inputHandler->SetMouseCapture(false);
		m_window->SetCursorVisibility(true);
		break;
	case 'Z':
		m_showCharts = !m_showCharts;
		break;
	case 'R':
		m_graphicsEngine->clearLights();
		entityManager->loadXML("lights.XML");
		break;
	case 'M':
		if (m_menu->isActive())
			m_menu->setActive(false);
		else
			m_menu->setActive(true);

		break;
	default:
		break;
	}
}

void Application::OnKeyUp(unsigned short key)
{
}

void Application::updateMenu(float dt)
{
	if (!m_menu->isActive())
		changeGameState(GameState::game);

	m_graphicsEngine->Begin2D();
	m_menu->draw(m_graphicsEngine);
	m_graphicsEngine->End2D();

	switch (m_menu->getStatus())
	{
	case Menu::resume:
		m_menu->setActive(false);
		changeGameState(GameState::game);
		break;
	case Menu::exit:
		m_quit = true;
		break;
	}
}

void Application::updateGame(float dt, Movement* playerMove)
{
	camera->Follow(entityManager->getEntityPos("player"));
	playerMove->setCamera(camera->GetLook(), camera->GetRight(), camera->GetUp());
	camera->Update(dt);
	this->entityManager->update(dt);
	m_graphicsEngine->UpdateScene(dt);
	m_graphicsEngine->DrawScene();

	if (m_menu->isActive())
		changeGameState(GameState::menu);
}

void Application::updateLoading(float dt)
{
	m_graphicsEngine->Begin2D();
	m_graphicsEngine->Draw2DTextureFile("..\\..\\content\\Textures\\Menygrafik\\fyraTreRatio.png", 0, 0);
	m_graphicsEngine->End2D();

	if (!levelHandler->isLoading())
		changeGameState(GameState::game);
}

void Application::changeGameState(GameState newState)
{
	if (m_window->IsActive())
	{
		if (newState == GameState::game)
		{
			m_inputHandler->SetMouseCapture(true);
			m_window->SetCursorVisibility(false);
		}
		else
		{
			m_inputHandler->SetMouseCapture(false);
			m_window->SetCursorVisibility(true);
		}
	}

	gameState = newState;
}
