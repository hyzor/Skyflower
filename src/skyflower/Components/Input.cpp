#include "Input.h"

// Must be included last!
#include "shared/debug.h"

Input::Input() : Component("Input")
{ 
}

Input::~Input()
{
}

void Input::addedToEntity()
{
	Entity *owner = getOwner();
	owner->getModules()->input->AddListener(this);

	getOwner()->sphere->Radius = 3.5f; //only player uses input
}

void Input::removeFromEntity()
{
	Entity *owner = getOwner();
	owner->getModules()->input->RemoveListener(this);
}

void Input::OnKeyDown(unsigned short key)
{
	switch (key)
	{
	case 'W':
		getOwner()->sendMessage("StartMoveForward", this);
		break;
	case 'S':
		getOwner()->sendMessage("StartMoveBackward", this);
		break;
	case 'A':
		getOwner()->sendMessage("StartMoveLeft", this);
		break;
	case 'D':
		getOwner()->sendMessage("StartMoveRight", this);
		break;
	case VK_SPACE:
		getOwner()->sendMessage("Jump", this);
		break;
	default:
		break;
	}
}

void Input::OnKeyUp(unsigned short key)
{
	switch (key)
	{
	case 'W':
		getOwner()->sendMessage("StopMoveForward", this);
		break;
	case 'S':
		getOwner()->sendMessage("StopMoveBackward", this);
		break;
	case 'A':
		getOwner()->sendMessage("StopMoveLeft", this);
		break;
	case 'D':
		getOwner()->sendMessage("StopMoveRight", this);
		break;
	case VK_SPACE:
		getOwner()->sendMessage("StopJump", this);
		break;
	default:
		break;
	}
}

void Input::OnMouseButtonDown(enum MouseButton button)
{
	switch (button)
	{
	case MouseButtonLeft:
		getOwner()->sendMessage("Throw", this);
		break;
	case MouseButtonRight:
		getOwner()->sendMessage("PickUpStart", this);
		break;
	default:
		break;
	}
}

void Input::OnMouseButtonUp(enum MouseButton button)
{
	switch (button)
	{
	case MouseButtonLeft:
		getOwner()->sendMessage("StopThrow", this);
		break;
	case MouseButtonRight:
		getOwner()->sendMessage("PickUpStop", this);
		break;
	default:
		break;
	}
}
