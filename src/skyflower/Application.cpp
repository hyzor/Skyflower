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


	ModelInstance* d = gEngine->CreateInstance("Data\\Models\\duck.obj");
	gEngine->CreateInstance("Data\\Models\\duck.obj");
	d->pos = Vec3(-70, 50, 0);

	float oldTime = GetTime();
	float time, timeElapsed;

	while(!m_quit)
	{
		time = GetTime();
		timeElapsed = time - oldTime;
		oldTime = time;

		d->pos += Vec3(0.01f, 0.0f, 0.0f);

		gEngine->DrawScene();
		gEngine->UpdateScene(timeElapsed);

		m_window->PumpMessages();
	}

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
