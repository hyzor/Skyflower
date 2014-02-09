#include "shared/util.h"

#include "Components/BoxComp.h"
#include "Components/Gravity.h"
#include "EntityManager.h"

// Must be included last!
#include "shared/debug.h"

void BoxComp::update(float dt)
{

	//rotate box
	Triangle gTriangle = getOwner()->getComponent<GravityComponent*>("Gravity")->GetGroundTriangle();
	Vec3 normal;
	if (gTriangle.P1 != Vec3())
	{
		normal = gTriangle.GetNormal();
		pNormal = normal;
	}
	else
		normal = pNormal;


	//räkna rotation längs med vectorn
	Vec3 normalX = normal;
	normalX.X = 0;
	normalX.Normalize();

	float rotx = asinf(normalX.Y);

	Vec3 normalZ = normal;
	normalZ.Z = 0;
	normalZ.Normalize();

	float rotz = asinf(normalZ.Y);

	getOwner()->updateRot(Vec3(rotx - 3.14 / 2, getOwner()->returnRot().Y, -rotz + 3.14 / 2));
}



