#ifndef COLLISIONINSTANCE_H
#define COLLISIONINSTANCE_H

#include "shared/Vec3.h"

#include "Box.h"
#include "Ray.h"
#include "Triangle.h"

class Sphere;

class CollisionInstance
{
public:
	virtual ~CollisionInstance() {}

	virtual void SetScale(Vec3 scale) = 0;
	virtual void SetRotation(Vec3 rot) = 0;
	virtual void SetPosition(Vec3 position) = 0;

	virtual float Test(Ray r) = 0;
	virtual bool Test(Triangle t) = 0;
	virtual bool Test(CollisionInstance* ci) = 0;

	virtual Box GetBox() = 0;
	virtual Sphere GetSphere() = 0;
	virtual Vec3 GetPosition() = 0;

	virtual bool isActive() = 0;
	virtual void setIsActive(bool status) = 0;
};

#endif
