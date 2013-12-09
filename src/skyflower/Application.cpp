#include "shared/util.h"
#include "EntityManager.h"
#include "Application.h"
#include "ScriptHandler.h"

Application::Application()
{
	m_window = NULL;
}

Application::~Application()
{
}

void Application::Start()
{
	m_window = new Window(1024, 768, L"Skyflower");
	m_window->SetListener(this);

	// Create graphics engine
	gEngine = CreateGraphicsEngine();
	gEngine->Init(m_window->GetHandle());

	m_quit = false;

	//loading xml-file, creating entities and components to this entityManager
	EntityManager * entityManager = new EntityManager();
	entityManager->loadXML(entityManager, "test3.xml");
	

	////sends a message to all components in all entities in that manager
	entityManager->sendMessageToAllEntities("Hello");

	////sends a message to a specific entity, in this case a Player-entity.
	//entityManager->sendMessageToEntity("Hello", "Player");

	//ScriptHandler *script = ScriptHandler::GetInstance();
	//script->Run("test.lua");

	gEngine->CreateInstance("Data\\Models\\duck.obj")->SetVisibility(false);

	ModelInstance* d = gEngine->CreateInstance("Data\\Models\\duck.obj", Vec3(-100, 50, 0));
	d->SetRotation(Vec3(-3.14f/2, 3.14f/4));

	float oldTime = GetTime();
	float time, timeElapsed;

	//if (script->getMessage())
	//{
	//	entityManager->sendMessageToAllEntities("Hello");
	//	entityManager->sendMessageToEntity("Hello", "Player2");
	//}

	while(!m_quit)
	{
		time = GetTime();
		timeElapsed = time - oldTime;
		oldTime = time;

		d->SetPosition(d->GetPosition() + Vec3(0.01f, 0.0f, 0.0f));


		gEngine->DrawScene();
		gEngine->UpdateScene(timeElapsed);

		m_window->PumpMessages();
	}

	gEngine->DeleteInstance(d);

	//DestroySoundEngine(m_soundEngine);

	//delete entityManager;

	delete m_window;
	DestroyGraphicsEngine(gEngine);
}

void Application::OnWindowShouldClose()
{
	m_quit = true;
}

void Application::OnWindowResize(unsigned int width, unsigned int height)
{
}
