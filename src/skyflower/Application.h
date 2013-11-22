#ifndef SKYFLOWER_APPLICATION
#define SKYFLOWER_APPLICATION

#include "graphics/Window.h"

class Application : WindowListener
{
public:
	Application();
	virtual ~Application();

	void Start();

public: // WindowListener
	void OnWindowShouldClose();
	void OnWindowResize(unsigned int width, unsigned int height);

private:
	Window *m_window;
	bool m_quit;
};

#endif
