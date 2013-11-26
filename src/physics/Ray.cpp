#include "Ray.h"


Ray::Ray()
{
}

Ray::Ray(float X, float Y, float Z, float dX, float dY, float dZ)
{
	this->Pos = Vec3(X, Y, Z);
	this->Dir = Vec3(dX, dY, dZ);
}

Ray::Ray(Vec3 Pos, Vec3 Dir)
{
	this->Pos = Pos;
	this->Dir = Dir;
}

Box Ray::GetBox()
{
	Vec3 v1 = Pos;
	Vec3 v2 = Pos + Dir;
	Vec3 min = Vec3::Min(v1, v2);
	Vec3 max = Vec3::Max(v1, v2);
	Box rayBounds = Box(min, max-min);
	return rayBounds;
}
