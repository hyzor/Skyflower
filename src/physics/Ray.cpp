#include "Ray.h"

// Must be included last!
#include "shared/debug.h"

Ray::Ray()
{
}

Ray::Ray(float X, float Y, float Z, float dX, float dY, float dZ)
{
	this->Pos = Vec3(X, Y, Z);
	this->Dir = Vec3(dX, dY, dZ);

	calcBox();
}

Ray::Ray(Vec3 Pos, Vec3 Dir)
{
	this->Pos = Pos;
	this->Dir = Dir;

	calcBox();
}

const Box& Ray::GetBox()
{
	return rayBounds;
}

void Ray::Set(const Vec3& pos, const Vec3& dir)
{
	this->Pos = pos;
	this->Dir = dir;

	calcBox();
}

const Vec3& Ray::GetPos()
{
	return this->Pos;
}

const Vec3& Ray::GetDir()
{
	return this->Dir;
}

void Ray::calcBox()
{
	Vec3 p = Pos;
	Vec3 s = Dir;

	if (s.X < 0)
	{
		p.X += s.X;
		s.X *= -1;
	}
	if (s.Y < 0)
	{
		p.Y += s.Y;
		s.Y *= -1;
	}
	if (s.Z < 0)
	{
		p.Z += s.Z;
		s.Z *= -1;
	}

	this->rayBounds.Position = p;
	this->rayBounds.Size = s;


	/*Vec3 v2 = Pos + Dir;
	Vec3 min = Vec3::Min(Pos, v2);
	Vec3 max = Vec3::Max(Pos, v2);
	this->rayBounds = Box(min, max - min);*/
}

