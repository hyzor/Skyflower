#ifndef COMPONENT_OSCILLATEPOSITION_H
#define COMPONENT_OSCILLATEPOSITION_H

#include <cstdio>
#include <string>

#include "shared/Vec3.h"
#include "Cistron.h"
#include "Entity.h"

using namespace std;
using namespace Cistron;

class OscillatePositionComponent : public Component {

public:
	OscillatePositionComponent(bool enabled, Vec3 direction, float speed, float travelDistance) : Component("OscillatePosition")
	{
		this->enabled = enabled;
		this->velocity = direction.Normalize() * speed;
		this->travelDistance = travelDistance;
	}

	virtual ~OscillatePositionComponent()
	{
	}

	void addedToEntity()
	{
		this->startPosition  = getEntityPos();
	}

	void removeFromEntity()
	{
	}

	void update(float deltaTime)
	{
		if (!this->enabled)
			return;

		Vec3 position = getEntityPos() + (this->velocity * deltaTime);

		if ((this->startPosition - position).Length() >= this->travelDistance) {
			this->velocity *= -1.0f;
		}

		updateEntityPos(position);
	}

	void setEnabled(bool enabled)
	{
		this->enabled = enabled;
	}

	bool isEnabled()
	{
		return this->enabled;
	}

private:
	bool enabled;
	Vec3 startPosition;
	Vec3 velocity;
	float travelDistance;
};

#endif