#ifndef COMPONENTS_LISTENER_H
#define COMPONENTS_LISTENER_H

#include <cassert>
#include <cstdio>

#include "shared/Vec3.h"
#include "Sound/SoundEngine.h"
#include "Sound/Listener.h"

#include "Cistron.h"
#include "Component.h"
#include "Entity.h"

using namespace std;
using namespace Cistron;

class ListenerComponent : public Component
{
public:
	ListenerComponent() : Component("Listener")
	{ 
	}

	virtual ~ListenerComponent()
	{
		Entity *owner = getOwner();

		assert(owner);

		owner->getModules()->sound->DestroyListener(m_listener);
	}

	void addedToEntity()
	{
		Entity *owner = getOwner();

		assert(owner);

		m_listener = owner->getModules()->sound->CreateListener();
		owner->getModules()->sound->SetActiveListener(m_listener);

		requestMessage("Update", &ListenerComponent::update);
	}

private:
	Listener *m_listener;

	void update(Message const& msg)
	{
		Vec3 position = getEntityPos();
		Vec3 rotation = getEntityRot();
		// FIXME: Derive forward from rotation.
		float forward[3] = {0.0f, 0.0f, 1.0f};
		float up[3] = {0.0f, 1.0f, 0.0f};

		m_listener->SetPosition(&position.X);
		m_listener->SetOrientation(forward, up);
	}
};

#endif
