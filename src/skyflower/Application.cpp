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
	m_window = new Window(800, 600, "Skyflower");
	m_window->SetListener(this);

	m_quit = false;

	float oldTime = GetTime();
	float time, timeElapsed;

	while(!m_quit)
	{
		time = GetTime();
		timeElapsed = time - oldTime;
		oldTime = time;

		m_window->PumpMessages();
	}

	delete m_window;
}

void Application::OnWindowShouldClose()
{
	m_quit = true;
}

void Application::OnWindowResize(unsigned int width, unsigned int height)
{
}
