#ifndef SKYFLOWER_INPUT_H
#define SKYFLOWER_INPUT_H

#include <string>

#include "Cistron.h"
#include "Entity.h"
#include "InputHandler.h"

using namespace Cistron;

class Input : public Component, InputListener
{
public:
	Input();
	virtual ~Input();

	void addedToEntity();
	void removeFromEntity();

public: // InputListener
	void OnKeyDown(unsigned short key);
	void OnKeyUp(unsigned short key);
	void OnMouseButtonDown(enum MouseButton button);
	void OnMouseButtonUp(enum MouseButton button);
};

#endif