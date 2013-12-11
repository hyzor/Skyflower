#include <cassert>
#include <cstring>

#include "shared/platform.h"
#include "shared/util.h"

#include "InputHandler.h"
#include "Window.h"

InputHandler::InputHandler(Window *window)
{
	m_window = window;
	m_inputDevicesRegistered = false;
	m_listener = NULL;
	m_mouseCaptured = false;

	for (int i = 0; i < ARRAY_SIZE(m_mouseButtonStates); i++)
	{
		m_mouseButtonStates[i] = false;
	}

	for (int i = 0; i < ARRAY_SIZE(m_keyStates); i++)
	{
		m_keyStates[i] = false;
	}
}

InputHandler::~InputHandler()
{
}

void InputHandler::SetListener(InputListener *listener)
{
	if (!m_inputDevicesRegistered)
	{
		RAWINPUTDEVICE devices[2];
		devices[0].usUsagePage = 0x01; // Generic desktop page
		devices[0].usUsage = 0x06;     // Keyboard
		devices[0].hwndTarget = m_window->GetHandle();
		devices[0].dwFlags = 0;
		devices[1].usUsagePage = 0x01; // Generic desktop page
		devices[1].usUsage = 0x02;     // Mouse
		devices[1].hwndTarget = m_window->GetHandle();
		devices[1].dwFlags = 0;

		BOOL result = RegisterRawInputDevices(devices, 2, sizeof(RAWINPUTDEVICE));

		assert(result == TRUE);

		m_inputDevicesRegistered = true;
	}

	m_listener = listener;
}

InputListener *InputHandler::GetListener() const
{
	return m_listener;
}

void InputHandler::Update()
{
	if (m_mouseCaptured && m_window->IsActive()) {
		POINT point;
		point.x = m_window->GetWidth() / 2;
		point.y = m_window->GetHeight() / 2;

		// Convert from window to screen coordinates.
		ClientToScreen(m_window->GetHandle(), &point);
		SetCursorPos(point.x, point.y);
	}
}

void InputHandler::SetMouseCapture(bool capture)
{
	m_mouseCaptured = capture;
}

bool InputHandler::IsMouseCaptured()
{
	return m_mouseCaptured;
}

bool InputHandler::isMouseButtonDown(enum MouseButton button) const
{
	return m_mouseButtonStates[button];
}

bool InputHandler::isKeyDown(unsigned short key) const
{
	return m_keyStates[key];
}

void InputHandler::GetMousePosition(int &x, int &y)
{
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(m_window->GetHandle(), &point);

	x = point.x;
	y = point.y;
}

bool InputHandler::isMouseInWindow()
{
	int x, y;
	GetMousePosition(x, y);

	if (x < 0 || y < 0)
		return false;
	if (x >= (int)m_window->GetWidth() || y >= (int)m_window->GetHeight())
		return false;

	return true;
}

void InputHandler::OnMouseMove(int deltaX, int deltaY)
{
	if (!m_mouseCaptured && !isMouseInWindow())
		return;

	if (m_listener)
		m_listener->OnMouseMove(deltaX, deltaY);
}

void InputHandler::OnMouseButtonDown(enum MouseButton button)
{
	if (!m_mouseCaptured && !isMouseInWindow())
		return;

	m_mouseButtonStates[button] = true;

	if (m_listener)
		m_listener->OnMouseButtonDown(button);
}

void InputHandler::OnMouseButtonUp(enum MouseButton button)
{
	if (!m_mouseCaptured && !isMouseInWindow())
		return;

	m_mouseButtonStates[button] = false;

	if (m_listener)
		m_listener->OnMouseButtonUp(button);
}

void InputHandler::OnMouseWheel(int delta)
{
	if (!m_mouseCaptured && !isMouseInWindow())
		return;

	if (m_listener)
		m_listener->OnMouseWheel(delta);
}

void InputHandler::OnKeyDown(unsigned short key)
{
	m_keyStates[key] = true;

	if (m_listener)
		m_listener->OnKeyDown(key);
}

void InputHandler::OnKeyUp(unsigned short key)
{
	m_keyStates[key] = false;

	if (m_listener)
		m_listener->OnKeyUp(key);
}
