#include "shared/util.h"

#include "Application.h"
#include <iostream>
#include <sstream>

Application::Application()
{
	m_window = NULL;
	m_soundEngine = NULL;

	pos = Vec3(0.0f, 0.0f, 0.0f);
	dir = Vec3(-1.0f, 0.0f, 0.0f);
	cameraLookAt = Vec3(pos - Vec3(0.0f, 5.0f, -10.0f));
	speed = 1.0f;
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
	std::stringstream ss;
	std::string x, y, z;
	p.update(0.01f);

	if (key == 'A')
	{
		p.move(pos, cameraLookAt, dir, speed);
	}
	

	ss << pos.X;
	x = ss.str();

	ss.str("");
	ss << pos.Y;
	y = ss.str();

	ss.str("");
	ss << pos.Z;
	z = ss.str();

	std::cout << "Pos X: " + x + " Y: " + y + " Z: " + z + "\n";
}

void Application::OnKeyUp(unsigned short key)
{
}
