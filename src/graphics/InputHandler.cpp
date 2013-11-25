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

	for (int i = 0; i < ARRAY_SIZE(m_mouseButtonStates); i++)
	{
		m_mouseButtonStates[i] = false;
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

bool InputHandler::isMouseButtonDown(enum MouseButton button) const
{
	return m_mouseButtonStates[button];
}

void InputHandler::OnMouseMove(int deltaX, int deltaY)
{
	if (m_listener)
		m_listener->OnMouseMove(deltaX, deltaY);
}

void InputHandler::OnMouseButtonDown(enum MouseButton button)
{
	m_mouseButtonStates[button] = true;

	if (m_listener)
		m_listener->OnMouseButtonDown(button);
}

void InputHandler::OnMouseButtonUp(enum MouseButton button)
{
	m_mouseButtonStates[button] = false;

	if (m_listener)
		m_listener->OnMouseButtonUp(button);
}

void InputHandler::OnMouseWheel(int delta)
{
	if (m_listener)
		m_listener->OnMouseWheel(delta);
}
