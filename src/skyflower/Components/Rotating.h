#ifndef COMPONENT_ROTATING_H
#define COMPONENT_ROTATING_H

#include <cstdio>
#include <string>

#include "shared/Vec3.h"
#include "Cistron.h"
#include "Entity.h"

using namespace std;
using namespace Cistron;

class RotatingComponent : public Component {

public:
	RotatingComponent(float yawSpeed, float pitchSpeed, float rollSpeed) : Component("Rotating")
	{
		this->yawSpeed = yawSpeed;
		this->pitchSpeed = pitchSpeed;
		this->rollSpeed = rollSpeed;
	}

	virtual ~RotatingComponent()
	{
	}

	void addedToEntity()
	{
	}

	void removeFromEntity()
	{
	}

	void update(float deltaTime)
	{
		Vec3 rotation = getEntityRot();
		rotation.X += this->pitchSpeed * deltaTime;
		rotation.Y += this->yawSpeed * deltaTime;
		rotation.Z += this->rollSpeed * deltaTime;

		updateEntityRot(rotation);
	}

private:
	float yawSpeed, pitchSpeed, rollSpeed;
};

#endif