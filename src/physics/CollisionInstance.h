#ifndef COLLISIONINSTANCE_H
#define COLLISIONINSTANCE_H

#include "shared/platform.h"
#include "CollisionModel.h"

class DLL_API CollisionInstance
{
	public:
		CollisionInstance(CollisionModel* Model, Vec3 Position);
		
		float Test(Ray r);
		bool Test(Triangle t);
		bool Test(CollisionInstance* ci);

		CollisionModel* Model;
		Vec3 Position;
	private:
};


#endif