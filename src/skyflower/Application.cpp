#include <string>
#include <iostream>
#include <cassert>

#include "shared/util.h"
#include "Sound/SoundEngine.h"

#include "Application.h"
#include "Cistron.h"
#include "tinyxml2.h"
#include "ComponentHeaders.h"

using namespace std;
using namespace tinyxml2;
using namespace Cistron;

Application::Application()
{
	m_window = NULL;
	m_soundEngine = NULL;
}

Application::~Application()
{
}

void Application::Start()
{
	//../../content/XML/test2.xml

	m_window = new Window(800, 600, "Skyflower");
	m_window->SetListener(this);

	m_inputHandler = m_window->GetInputHandler();
	m_inputHandler->SetListener(this);

	EntityManager *entityManager = new EntityManager();

	//loading xml-file, creating entities and components to this entityManager
	//entityManager->loadXML(entityManager, "test2.xml");

	////sends a message to all components in all entities in that manager
	//entityManager->sendMessageToAllEntities("Hello");

	////sends a message to a specific entity, in this case a Player-entity.
	//entityManager->sendMessageToEntity("Hello", "Player");

	//m_soundEngine = CreateSoundEngine();

	m_quit = false;

	float oldTime = GetTime();
	float time, deltaTime;

	while(!m_quit)
	{
		time = GetTime();
		deltaTime = time - oldTime;
		oldTime = time;

		m_window->PumpMessages();
	}

	//DestroySoundEngine(m_soundEngine);

	delete entityManager;

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
}

void Application::OnKeyUp(unsigned short key)
{
}
