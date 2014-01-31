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
		sendMessageToEntity(this->getOwnerId(), "StartMoveForward");
		break;
	case 'S':
		sendMessageToEntity(this->getOwnerId(), "StartMoveBackward");
		break;
	case 'A':
		sendMessageToEntity(this->getOwnerId(), "StartMoveLeft");
		break;
	case 'D':
		sendMessageToEntity(this->getOwnerId(), "StartMoveRight");
		break;
	case VK_SPACE:
		sendMessageToEntity(this->getOwnerId(), "Jump");
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
		sendMessageToEntity(this->getOwnerId(), "StopMoveForward");
		break;
	case 'S':
		sendMessageToEntity(this->getOwnerId(), "StopMoveBackward");
		break;
	case 'A':
		sendMessageToEntity(this->getOwnerId(), "StopMoveLeft");
		break;
	case 'D':
		sendMessageToEntity(this->getOwnerId(), "StopMoveRight");
		break;
	case VK_SPACE:
		sendMessageToEntity(this->getOwnerId(), "StopJump");
		break;
	default:
		break;
	}
}
