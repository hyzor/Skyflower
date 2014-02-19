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

class GravityComponent : public Component
{
public:
	GravityComponent();
	virtual ~GravityComponent();

	void addedToEntity();
	void removeFromEntity();

	void update(float deltaTime);

	void setEnabled(bool enabled);
	bool isEnabled();
	const Vec3 *GetGroundNormal();

private:
	float testMove(Ray r, Entity* e, Entity* &out);
	void createRays();
	void calculateGroundNormal(Entity* e, Entity* ground);
	void sphereCollision(float dt);

private:
	PhysicsEntity* p;
	bool enabled;
	Vec3 groundNormal;
	bool foundGroundNormal;

	vector<Ray> groundRays;
	vector<Ray> wallRays;
};

#endif
