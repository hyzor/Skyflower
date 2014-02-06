#include "shared/util.h"

#include "Components/BoxComp.h"
#include "EntityManager.h"

// Must be included last!
#include "shared/debug.h"

void BoxComp::update(float dt)
{

	//rotate box
	Triangle gTriangle = getGroundTriangle(getOwner(), getOwner()->ground);
	Vec3 normal = gTriangle.GetNormal();

	if (gTriangle.P1 != Vec3())
	{
		Vec3 normalX = normal;
		normalX.X = 0;
		normalX.Normalize();

		float rotx = asinf(normalX.Y);

		Vec3 normalZ = normal;
		normalZ.Z = 0;
		normalZ.Normalize();

		float rotz = asinf(normalZ.Y);

		getOwner()->updateRot(Vec3(-rotx + 3.14 / 2, getOwner()->returnRot().Y, -rotz + 3.14 / 2));
	}
	

}


Triangle BoxComp::getGroundTriangle(Entity* e, Entity* ground)
{
	
	Vec3 pos = e->returnPos();

	//rays for each point in the triangle
	Ray left = Ray(Vec3(-3, 5, 3)+pos, Vec3(0, -10, 0));
	Ray right = Ray(Vec3(3, 5, 3)+pos, Vec3(0, -10, 0));
	Ray bottom = Ray(Vec3(0, 5, -3)+pos, Vec3(0, -10, 0));

	if (!ground)
		return Triangle(left.GetPos(), right.GetPos(), bottom.GetPos());

	//find collision for each ray
	float tLeft = ground->collInst->Test(left);
	float tRight = ground->collInst->Test(right);
	float tBottom = ground->collInst->Test(bottom);

	if (tLeft == 0 || tRight == 0 || tLeft == 0)
		return Triangle();


	//calculate triangle
	Vec3 pointL = left.GetDir();
	pointL *= tLeft;
	pointL += left.GetPos();

	Vec3 pointR = right.GetDir();
	pointR *= tRight;
	pointR += right.GetPos();

	Vec3 pointB = bottom.GetDir();
	pointB *= tBottom;
	pointB += bottom.GetPos();

	return Triangle(pointL, pointR, pointB);
}
