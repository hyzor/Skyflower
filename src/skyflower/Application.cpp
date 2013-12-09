#include "shared/util.h"
#include "Application.h"

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
	CameraController *camera = gEngine->CreateCameraController();
	//loading xml-file, creating entities and components to this entityManager
	//entityManager->loadXML(entityManager, "test2.xml");

	////sends a message to all components in all entities in that manager
	//entityManager->sendMessageToAllEntities("Hello");

	////sends a message to a specific entity, in this case a Player-entity.
	//entityManager->sendMessageToEntity("Hello", "Player");


	gEngine->CreateInstance("Data\\Models\\duck.obj")->SetVisibility(false);

	ModelInstance* d = gEngine->CreateInstance("Data\\Models\\duck.obj", Vec3(-100, 50, 0));
	d->SetRotation(Vec3(-3.14f/2, 3.14f/4));

	float oldTime = GetTime();
	float time, timeElapsed;

	while(!m_quit)
	{
		time = GetTime();
		timeElapsed = time - oldTime;
		oldTime = time;

		d->SetPosition(d->GetPosition() + Vec3(0.01f, 0.0f, 0.0f));
		Vec3 dir = (d->GetPosition() - camera->GetPosition()).Normalize();

		camera->SetDirection(dir);
		camera->SetPosition(d->GetPosition() - dir * 1000);
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
