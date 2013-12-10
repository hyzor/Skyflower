#include <string>
#include <iostream>
#include <cassert>

#include "shared/util.h"
#include "Sound/SoundEngine.h"

#include "Application.h"
#include "tinyxml2.h"
#include "ComponentHeaders.h"

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
	m_inputHandler->SetListener(this);

	// Create graphics engine
	m_graphicsEngine = CreateGraphicsEngine();
	m_graphicsEngine->Init(m_window->GetHandle(), m_window->GetWidth(), m_window->GetHeight());

	//m_soundEngine = CreateSoundEngine("../../content/sounds/");
	//assert(m_soundEngine);

	entityManager = new EntityManager(m_graphicsEngine);

	//loading xml-file, creating entities and components to this entityManager
	entityManager->loadXML(entityManager, "test2.xml");

	////sends a message to all components in all entities in that manager
	//entityManager->sendMessageToAllEntities("Hello");

	////sends a message to a specific entity, in this case a Player-entity.
	//entityManager->sendMessageToEntity("Hello", "Player");



	CameraController *camera = m_graphicsEngine->CreateCameraController();
	
	//m_graphicsEngine->CreateInstance("Data\\Models\\duck.obj")->SetVisibility(false);

	//ModelInstance* d = m_graphicsEngine->CreateInstance("Data\\Models\\duck.obj", Vec3(-100, 50, 0));
	//d->SetRotation(Vec3(-3.14f/2, 3.14f/4));



	float oldTime = GetTime();
	float time, deltaTime;

	m_quit = false;

	while(!m_quit)
	{
		time = GetTime();
		deltaTime = time - oldTime;
		oldTime = time;



		//d->SetPosition(d->GetPosition() + Vec3(0.01f, 0.0f, 0.0f));
		//Vec3 dir = (d->GetPosition() - camera->GetPosition()).Normalize();

		//camera->SetDirection(dir);
		//camera->SetPosition(d->GetPosition() - dir * 1000);



		m_graphicsEngine->DrawScene();
		m_graphicsEngine->UpdateScene(deltaTime);

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
}

void Application::OnMouseMove(int deltaX, int deltaY)
{
}

void Application::OnMouseButtonDown(enum MouseButton button)
{
}

void Application::OnMouseButtonUp(enum MouseButton button)
{
}

void Application::OnMouseWheel(int delta)
{
}

void Application::OnKeyDown(unsigned short key)
{
	switch (key)
	{
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
