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
	LineChart frameTimeChart(1024 * 1024);
	frameTimeChart.SetSize(512, 256);
	frameTimeChart.SetUnit("ms");

	LineChart memoryChart(1024 * 1024);
	memoryChart.SetSize(512, 256);
	memoryChart.SetUnit("MiB");



	m_window = new Window(1024, 768, L"Skyflower");
	m_window->SetListener(this);

	m_inputHandler = m_window->GetInputHandler();
	m_inputHandler->AddListener(this);

	// Create graphics engine
	m_graphicsEngine = CreateGraphicsEngine();
	m_graphicsEngine->Init(m_window->GetHandle(), m_window->GetWidth(), m_window->GetHeight());

	//m_soundEngine = CreateSoundEngine("../../content/sounds/");
	//assert(m_soundEngine);

	entityManager = new EntityManager("../../content/XML/", m_graphicsEngine);

	//loading xml-file, creating entities and components to this entityManager
	entityManager->loadXML(entityManager, "test22.xml");
	entityManager->loadXML(entityManager, "platform2.xml");

	////sends a message to all components in all entities in that manager
	//entityManager->sendMessageToAllEntities("Hello");

	////sends a message to a specific entity, in this case a Player-entity.
	//entityManager->sendMessageToEntity("Hello", "Player");

	camera = m_graphicsEngine->CreateCameraController();
	Cistron::Component* playerMove = (Movement*)entityManager->getComponent("Player", "Movement");

	//m_graphicsEngine->CreateInstance("Data\\Models\\duck.obj")->SetVisibility(false);

	//ModelInstance* d = m_graphicsEngine->CreateInstance("Data\\Models\\duck.obj", Vec3(-100, 50, 0));
	//d->SetRotation(Vec3(-3.14f/2, 3.14f/4));


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

		camera->Follow(entityManager->getEntityPos("Player"));
		camera->Update();

		frameTimeChart.AddPoint((float)time, (float)(deltaTime * 1000.0));
		memoryChart.AddPoint((float)time, GetMemoryUsage() / (1024.0f * 1024.0f));
		
		if (time - startTime > chartTime) {
			startTime = time;

			//frameTimeChart.Draw((float)(time - chartTime), (float)time, 1.0f / 100.0f, (1.0f / 60.0f) * 1000.0f);
			//memoryChart.Draw((float)(time - chartTime), (float)time, 1.0f / 100.0f, 256.0f);
		}
		this->entityManager->sendMessageToEntity("Update", "Player");


		m_graphicsEngine->DrawScene();
		m_graphicsEngine->UpdateScene((float)deltaTime);

		//m_soundEngine->Update(deltaTime);

		m_window->PumpMessages();
	}

	//m_graphicsEngine->DeleteInstance(d);



	delete entityManager;
	//DestroySoundEngine(m_soundEngine);
	DestroyGraphicsEngine(m_graphicsEngine);
	delete m_window;
}

void Application::OnWindowShouldClose()
{
	m_quit = true;
}

void Application::OnWindowResize(unsigned int width, unsigned int height)
{
	// FIXME: Notify the graphics engine of the resize.
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
	camera->RotateCamera(deltaX, deltaY);
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
	camera->Zoom(delta, 50);
}

void Application::OnKeyDown(unsigned short key)
{
	switch (key)
	{
	case VK_ESCAPE:
		m_inputHandler->SetMouseCapture(false);
		m_window->SetCursorVisibility(true);
		break;
	case 'W':
		entityManager->sendMessageToEntity("W", "Player");
		break;
	case 'S':
		entityManager->sendMessageToEntity("S", "Player");
		break;
	case 'A':
		entityManager->sendMessageToEntity("A", "Player");
		break;
	case 'D':
		entityManager->sendMessageToEntity("D", "Player");
		break;
	case 'E':
		entityManager->sendMessageToEntity("E", "Player");
		break;
	}
}

void Application::OnKeyUp(unsigned short key)
{
}
