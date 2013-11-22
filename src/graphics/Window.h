#ifndef GRAPHICS_WINDOW_H
#define GRAPHICS_WINDOW_H

#include "shared/platform.h"

class DLL_API WindowListener
{
public:
	virtual void OnWindowShouldClose() = 0;
	virtual void OnWindowResize(unsigned int width, unsigned int height) = 0;
};

class DLL_API Window
{
public:
	Window(const unsigned int width, const unsigned int height, const char *title);
	virtual ~Window();

	void PumpMessages() const;

	void SetListener(WindowListener *listener);

	HWND GetHandle() const;
	WindowListener *GetListener() const;
	unsigned int GetWidth() const;
	unsigned int GetHeight() const;

private:
	static const char *m_windowClassName;

	HINSTANCE m_instanceHandle;
	HWND m_window;

	WindowListener *m_listener;
};

#endif
