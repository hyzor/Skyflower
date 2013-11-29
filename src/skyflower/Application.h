#ifndef SKYFLOWER_APPLICATION
#define SKYFLOWER_APPLICATION

#include "graphics/Window.h"
#include "graphics/InputHandler.h"
#include "Sound/SoundEngine.h"
#include "physics/Physics.h"
#include <iostream>
#include <sstream>

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

	void OnKeyDown(unsigned short key);
	void OnKeyUp(unsigned short key);

private:
	Window *m_window;
	InputHandler *m_inputHandler;

	SoundEngine *m_soundEngine;

	Physics p;
	Vec3 pos; //representing the position of a movable object
	Vec3 look; //representing the forward direction of a movable object
	Vec3 right; //representing the right direction of a movable object
	float speed;

	bool m_quit;
};

#endif
