#ifndef COMPONENTS_FOOTSTEPS_H
#define COMPONENTS_FOOTSTEPS_H

#include "shared/Vec3.h"

#include "Cistron.h"
#include "Component.h"

using namespace Cistron;

class FootstepsComponent : public Component
{
public:
	FootstepsComponent();
	virtual ~FootstepsComponent();

	void addedToEntity();
	void update(float deltaTime);

private:
	Vec3 m_oldPosition;
	float m_distanceTraveled;
};

#endif
