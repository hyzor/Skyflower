#ifndef SKYFLOWER_APPLICATION
#define SKYFLOWER_APPLICATION

#include "graphics/Window.h"
#include "graphics/InputHandler.h"

class Application : WindowListener, InputListener
{
public:
	Application();
	virtual ~Application();

	void Start();

public: // WindowListener
	void OnWindowShouldClose();
	void OnWindowResize(unsigned int width, unsigned int height);

public: // InputListener
	void OnMouseMove(int deltaX, int deltaY);
	void OnMouseButtonDown(enum MouseButton button);
	void OnMouseButtonUp(enum MouseButton button);
	void OnMouseWheel(int delta);

private:
	Window *m_window;
	InputHandler *m_inputHandler;
	bool m_quit;
};

#endif
