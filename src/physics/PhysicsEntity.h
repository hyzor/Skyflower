#ifndef PHYSICSENTITY_H
#define PHYSICSENTITY_H

#include "Vec3.h"
#include "shared/platform.h"


class DLL_API PhysicsEntity
{
public:
	PhysicsEntity(long id, float mass);
	~PhysicsEntity();
	void Update(float dt);
	void Jump();

private:
	long id;
	float mass;
	bool jumping;
	Vector rotation;

};
#endif