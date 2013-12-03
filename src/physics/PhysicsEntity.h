#ifndef PHYSICSENTITY_H
#define PHYSICSENTITY_H

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

};
#endif