#include <string.h>

#include "shared/platform.h"

#include "Window.h"

const char *Window::m_windowClassName = "Skyflower";

static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Window *window = (Window *)GetWindowLongPtr(hWnd, 0);

	switch(uMsg)
	{
		case WM_CREATE:
			SetWindowLongPtr(hWnd, 0, (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams);
			break;
		case WM_SIZE:
			window->GetListener()->OnWindowResize((unsigned int)LOWORD(lParam), (unsigned int)HIWORD(lParam));
			break;
		case WM_CLOSE:
			window->GetListener()->OnWindowShouldClose();
			return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

Window::Window(const unsigned int width, const unsigned int height, const char *title)
{
	WNDCLASSEX windowClass;

	m_instanceHandle = GetModuleHandle(NULL);

	memset(&windowClass, 0, sizeof(windowClass));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW; // | CS_OWNDC
	windowClass.lpfnWndProc = WindowProc;
	windowClass.cbWndExtra = sizeof(void *); // Extra space for the Window pointer.
	windowClass.hInstance = m_instanceHandle;
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = NULL;
	windowClass.lpszClassName = m_windowClassName;

	RegisterClassEx(&windowClass);

	DWORD windowStyle = WS_OVERLAPPEDWINDOW;

	// The size passed to CreateWindow includes the size of the borders, so take the borders
	// in account so that windowWidth and windowHeight will create a window with a drawable
	// surface of the specified size.
	RECT rect;
	SetRect(&rect, 0, 0, width, height);
	AdjustWindowRect(&rect, windowStyle, false);

	unsigned int windowWidth = rect.right - rect.left;
	unsigned int windowHeight = rect.bottom - rect.top;

	// Clamp windowWidth and windowHeight to the desktop dimensions.
	unsigned int screenWidth = (unsigned int)GetSystemMetrics(SM_CXSCREEN);
	unsigned int screenHeight = (unsigned int)GetSystemMetrics(SM_CYSCREEN);

	if (windowWidth > screenWidth)
		windowWidth = screenWidth;
	if (windowHeight > screenHeight)
		windowHeight = screenHeight;

	m_window = CreateWindow(m_windowClassName,   // window class
						    title,               // title-bar string
							windowStyle,         // style
						    CW_USEDEFAULT,       // horizontal position
						    CW_USEDEFAULT,       // vertical position
							windowWidth,		 // width
							windowHeight,		 // height
						    NULL,				 // parent window
						    NULL,				 // menu
							m_instanceHandle,    // handle to application instance
						    this);		         // window-creation data (this will be passed to the WM_CREATE message)

	ShowWindow(m_window, SW_SHOW);

	SetForegroundWindow(m_window);
	SetFocus(m_window);
}

Window::~Window()
{
	DestroyWindow(m_window);
	UnregisterClass(m_windowClassName, m_instanceHandle);
}

void Window::PumpMessages() const
{
	MSG msg;

	while(PeekMessage(&msg, m_window, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void Window::SetListener(WindowListener *listener)
{
	m_listener = listener;
}

WindowListener *Window::GetListener() const
{
	return m_listener;
}

unsigned int Window::GetWidth() const
{
	RECT rect;
	GetClientRect(m_window, &rect);

	return rect.right;
}

unsigned int Window::GetHeight() const
{
	RECT rect;
	GetClientRect(m_window, &rect);

	return rect.bottom;
}
