#ifndef COMPONENTS_LISTENER_H
#define COMPONENTS_LISTENER_H

#include "Sound/Listener.h"

#include "Cistron.h"
#include "Component.h"

using namespace Cistron;

class ListenerComponent : public Component
{
public:
	ListenerComponent();
	virtual ~ListenerComponent();

	void addedToEntity();
	void removeFromEntity();

	void update(float deltaTime);

	void activateListener(Message const& msg);

private:
	Listener *m_listener;
};

#endif
