#ifndef SKYFLOWER_APPLICATION
#define SKYFLOWER_APPLICATION

#include "graphics/Window.h"
#include "graphics/InputHandler.h"
#include "graphics/GraphicsEngine.h"
#include "shared/util.h"
#include "PotentialField.h"
#include "physics/Collision.h"
#include "Sound/SoundEngine.h"
#include "Cistron.h"

class Application : WindowListener, InputListener
{
public:
	Application();
	virtual ~Application();

	void Start();

public: // WindowListener
	void OnWindowShouldClose();
	void OnWindowResize(unsigned int width, unsigned int height);
	void OnWindowActivate();
	void OnWindowDeactivate();

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

	GraphicsEngine *m_graphicsEngine;
	SoundEngine *m_soundEngine;
	EntityManager* entityManager;
	CameraController *camera;
	bool m_quit;
};

#endif
