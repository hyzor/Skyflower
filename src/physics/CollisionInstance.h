#ifndef COLLISIONINSTANCE_H
#define COLLISIONINSTANCE_H

#include "shared/platform.h"
#include "CollisionModel.h"

class DLL_API CollisionInstance
{
	public:
		CollisionInstance(CollisionModel* Model, Vec3 Position);
		
		float Test(Ray r);

		CollisionModel* Model;
		Vec3 Position;
	private:
};


#endif