#ifndef COMPONENT_GRAVITY_H
#define COMPONENT_GRAVITY_H

#include "Cistron.h"
#include <string>
#include <iostream>
#include <sstream>
#include "shared/Vec3.h"
#include "physics/PhysicsEngine.h"
#include "physics/Collision.h"
#include "Entity.h"
using namespace std;
using namespace Cistron;

class GravityComponent : public Component {

public:
	GravityComponent() : Component("Gravity")
	{
		this->enabled = true;

	}

	virtual ~GravityComponent()
	{
	}

	void addedToEntity()
	{
		this->p = getOwner()->getPhysics();

		createRays();
	}

	void removeFromEntity()
	{
		this->p = NULL;
	}

	void update(float deltaTime);

	void setEnabled(bool enabled)
	{
		this->enabled = enabled;
	}

	bool isEnabled()
	{
		return this->enabled;
	}

private:
	PhysicsEntity* p;
	bool enabled;

	vector<Ray> groundRays;
	vector<Ray> wallRays;

	float testMove(Ray r, Entity* e, Entity* &out);

	void createRays();
};

#endif
