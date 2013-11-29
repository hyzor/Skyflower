#include "shared/util.h"

#include "Application.h"

Application::Application()
{
	m_window = NULL;
	m_soundEngine = NULL;

	this->look = Vec3(1.0f, 0.0f, 1.0f);
	this->pos = Vec3(0.0f, 0.0f, 0.0f);
	this->right = Vec3(1.0f, 0.0f, -1.0f);
	this->speed = 1.0f;
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
	switch (key)
	{
	case 0x57:
		p.moveForward(pos, look, speed);
		break;
	case 0x53:
		p.moveBackward(pos, look, speed);
		break;
	case 0x41:
		p.moveLeft(pos, right, speed);
		break;
	case 0x44:
		p.moveRight(pos, right, speed);
		break;
	}

	std::ostringstream ss;
	std::string x, y, z;

	ss << pos.X;
	x = ss.str();

	ss.str("");
	ss << pos.Y;
	y = ss.str();

	ss.str("");
	ss << pos.Z;
	z = ss.str();

	std::cout << "Position X: " + x + ", Y: " + y + " Z: " + z + "\n";
	p.update(0.1f);
}

void Application::OnKeyUp(unsigned short key)
{
}
