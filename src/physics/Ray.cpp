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
