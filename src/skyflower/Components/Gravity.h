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
	bool GroundRayColliding(int index);

	const Vec3 *GetGroundNormal();

	int NrOfGroundRays();
	Ray GroundRay(int index);


private:
	float testMove(Ray r, Entity* e, Entity* &out, bool groundRay, int index);
	void createRays();
	void calculateGroundNormal(Entity* e, Entity* ground);

private:

	PhysicsEntity* p;

	bool enabled;
	bool foundGroundNormal;

	Vec3 groundNormal;
	Vec3 prevPos;

	vector<Ray> groundRays;
	vector<bool> isGroundColl;
	vector<Ray> wallRays;
};

#endif
