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

	Modules modules;
	modules.input = m_inputHandler;
	modules.graphics = m_graphicsEngine;
	modules.sound = m_soundEngine;
	modules.potentialField = new PotentialField();
	
	entityManager = new EntityManager("../../XML/", &modules);

	entityManager->loadXML2("player.xml");
	levelHandler->init(entityManager);
	//entityManager->loadXML2("player2.xml");
	//entityManager->loadXML2("platform.xml");
	//entityManager->loadXML2("block22.xml");
	//entityManager->loadXML2("TriggerTest.xml");
	//entityManager->loadXML2("Player3.xml");

	// Load Hub Level
	levelHandler->load(1);

	camera = m_graphicsEngine->CreateCameraController();
	Movement* playerMove = (Movement*)entityManager->getComponent("player", "Movement");

	entityManager->sendMessageToEntity("ActivateListener", "player");

	LineChart frameTimeChart(1024 * 1024);
	frameTimeChart.SetSize(512, 256);
	frameTimeChart.SetUnit("ms");

	LineChart memoryChart(1024 * 1024);
	memoryChart.SetSize(512, 256);
	memoryChart.SetUnit("MiB");

	double startTime = GetTime();
	double chartTime = 5.0;

	double oldTime = GetTime();
	double time, deltaTime;

	m_quit = false;

	while(!m_quit)
	{
		time = GetTime();
		deltaTime = time - oldTime;
		oldTime = time;

		frameTimeChart.AddPoint((float)time, (float)(deltaTime * 1000.0));
		memoryChart.AddPoint((float)time, GetMemoryUsage() / (1024.0f * 1024.0f));
		
		if (time - startTime > chartTime) {
			startTime = time;

			//frameTimeChart.Draw((float)(time - chartTime), (float)time, 1.0f / 100.0f, (1.0f / 60.0f) * 1000.0f);
			//memoryChart.Draw((float)(time - chartTime), (float)time, 1.0f / 100.0f, 256.0f);
		}

		camera->Follow(entityManager->getEntityPos("player"));
		playerMove->setCamera(camera->GetLook(), camera->GetRight(), camera->GetUp());
		camera->Update((float)deltaTime);

		this->entityManager->update((float)deltaTime);
		//this->entityManager->handleCollision();

		m_graphicsEngine->Run((float)deltaTime);

		m_soundEngine->Update((float)deltaTime);

		m_window->PumpMessages();
	}

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
	m_inputHandler->SetMouseCapture(true);
	m_window->SetCursorVisibility(false);
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
	if (!m_inputHandler->IsMouseCaptured()) {
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
	switch (key)
	{
	case VK_ESCAPE:
		m_inputHandler->SetMouseCapture(false);
		m_window->SetCursorVisibility(true);
		break;
	default:
		break;
	}
}

void Application::OnKeyUp(unsigned short key)
{
}
