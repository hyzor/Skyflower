#ifndef COLLISIONINSTANCEIMPL_H
#define COLLISIONINSTANCEIMPL_H

#include "CollisionInstance.h"

#include "CollisionModel.h"

class CollisionInstanceImpl : public CollisionInstance
{
	public:
		CollisionInstanceImpl(CollisionModel* Model, Vec3 Position);
		virtual ~CollisionInstanceImpl();

		void SetScale(Vec3 scale);
		void SetRotation(Vec3 rot);
		void SetPosition(Vec3 position);

		float Test(Ray r);
		bool Test(Triangle t);
		bool Test(CollisionInstance* ci);

		CollisionModel *GetModel();
		Box GetBox();
		Sphere GetSphere();
		Vec3 GetPosition();

		bool isActive();
		void setIsActive(bool status);

	private:
		CollisionModel* Model;
		
		bool _isActive;

		Vec3 Position;
		Vec3 scaleInv;
		Vec3 rot1Inv;
		Vec3 rot2Inv;
		Vec3 rot3Inv;
};

#endif
