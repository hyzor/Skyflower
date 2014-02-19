#define _USE_MATH_DEFINES
#include <cmath>

#include "shared/util.h"

#include "Components/BoxComp.h"
#include "Components/Gravity.h"
#include "EntityManager.h"

// Must be included last!
#include "shared/debug.h"

BoxComp::BoxComp(float speed) : Component("Box")
{
	this->speed = speed;

	pNormal = Vec3(0, 1, 0);
}

BoxComp::~BoxComp()
{
}

void BoxComp::addedToEntity()
{

}

void BoxComp::removeFromEntity()
{
}

void BoxComp::update(float dt)
{
	//rotate box
	const Vec3 *groundNormal = getOwner()->getComponent<GravityComponent*>("Gravity")->GetGroundNormal();
	Vec3 normal;

	if (groundNormal)
	{
		normal = *groundNormal;
		pNormal = normal;
	}
	else
	{
		normal = pNormal;
	}

	//räkna rotation längs med vectorn
	float rotx = asinf(Vec3(0.0f, normal.Y, normal.Z).Normalize().Y);
	float rotz = asinf(Vec3(normal.X, normal.Y, 0.0f).Normalize().Y);

	getOwner()->updateRot(Vec3(rotx - M_PI_2, getOwner()->returnRot().Y, -rotz + M_PI_2));
}

float BoxComp::GetSpeed()
{
	return speed;
}
