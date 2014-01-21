#ifndef COLLISIONINSTANCE_H
#define COLLISIONINSTANCE_H

#include "shared/platform.h"
#include "CollisionModel.h"

class DLL_API CollisionInstance
{
	public:
		CollisionInstance(CollisionModel* Model, Vec3 Position);

		void SetScale(Vec3 scale);
		void SetRotation(Vec3 rot);
		
		float Test(Ray r);
		bool Test(Triangle t);
		bool Test(CollisionInstance* ci);

		Box GetBox();

		CollisionModel* Model;
		Vec3 Position;
	private:

		Vec3 scaleInv;
		Vec3 rot1Inv;
		Vec3 rot2Inv;
		Vec3 rot3Inv;
};


#endif