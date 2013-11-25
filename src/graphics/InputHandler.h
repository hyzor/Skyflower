#ifndef GRAPHICS_INPUTHANDLER_H
#define GRAPHICS_INPUTHANDLER_H

#include "shared/platform.h"

class Window;

enum MouseButton
{
	MouseButtonLeft = 0,
	MouseButtonRight,
	MouseButtonMiddle,
	MouseButtonCount
};

class DLL_API InputListener
{
public:
	virtual void OnMouseMove(int deltaX, int deltaY) = 0;
	virtual void OnMouseButtonDown(enum MouseButton button) = 0;
	virtual void OnMouseButtonUp(enum MouseButton button) = 0;
	virtual void OnMouseWheel(int delta) = 0;
};

class DLL_API InputHandler
{
public:
	virtual ~InputHandler();

	void SetListener(InputListener *listener);

	InputListener *GetListener() const;

	bool isMouseButtonDown(enum MouseButton button) const;


	void OnMouseMove(int deltaX, int deltaY);
	void OnMouseButtonDown(enum MouseButton button);
	void OnMouseButtonUp(enum MouseButton button);
	void OnMouseWheel(int delta);

private:
	friend class Window;

	InputHandler(Window *window);

private:
	Window *m_window;
	bool m_inputDevicesRegistered;
	InputListener *m_listener;

	bool m_mouseButtonStates[MouseButtonCount];
};

#endif
