#include "shared/util.h"

#include "Application.h"

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
	m_window = new Window(800, 600, "Skyflower");
	m_window->SetListener(this);

	m_inputHandler = m_window->GetInputHandler();
	m_inputHandler->SetListener(this);

	//m_soundEngine = CreateSoundEngine();

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

	//DestroySoundEngine(m_soundEngine);

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
