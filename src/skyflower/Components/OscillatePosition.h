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
	OscillatePositionComponent(Vec3 direction, float speed, float travelDistance) : Component("Movement")
	{
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
		Vec3 position = getEntityPos() + (this->velocity * deltaTime);

		if ((this->startPosition - position).Length() >= this->travelDistance) {
			this->velocity *= -1.0f;
		}

		updateEntityPos(position);
	}

private:
	Vec3 startPosition;
	Vec3 velocity;
	float travelDistance;
};

#endif