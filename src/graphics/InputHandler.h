#ifndef GRAPHICS_INPUTHANDLER_H
#define GRAPHICS_INPUTHANDLER_H

#include <climits>

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

	// key is one of the virtual-key codes listed at http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
	virtual void OnKeyDown(unsigned short key) = 0;
	virtual void OnKeyUp(unsigned short key) = 0;
};

class DLL_API InputHandler
{
public:
	virtual ~InputHandler();

	void SetListener(InputListener *listener);

	InputListener *GetListener() const;

	void SetMouseCapture(bool capture);
	bool GetMouseCapture();

	bool isMouseButtonDown(enum MouseButton button) const;
	bool isKeyDown(unsigned short key) const;
	// The returned values are relative to the upper-left corner of the window.
	void GetMousePosition(int &x, int &y);
	bool isMouseInWindow();

	// Only WindowProc should call these.
	void OnMouseMove(int deltaX, int deltaY);
	void OnMouseButtonDown(enum MouseButton button);
	void OnMouseButtonUp(enum MouseButton button);
	void OnMouseWheel(int delta);

	void OnKeyDown(unsigned short key);
	void OnKeyUp(unsigned short key);

private:
	friend class Window;

	InputHandler(Window *window);

private:
	Window *m_window;
	bool m_inputDevicesRegistered;
	InputListener *m_listener;
	bool m_mouseCaptured;

	bool m_mouseButtonStates[MouseButtonCount];
	bool m_keyStates[USHRT_MAX];
};

#endif
