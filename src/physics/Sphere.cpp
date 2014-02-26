#include "Sphere.h"

// Must be included last!
#include "shared/debug.h"

Sphere::Sphere()
{
	this->Radius = 0;
}

Sphere::Sphere(float X, float Y, float Z)
{
	this->Position = Vec3(X, Y, Z);
	this->Radius = 0;
}

Sphere::Sphere(float X, float Y, float Z, float Radius)
{
	this->Position = Vec3(X, Y, Z);
	this->Radius = Radius;
}

Sphere::Sphere(Vec3 position, float Radius)
{
	this->Position = position;
	this->Radius = Radius;
}

Sphere::Sphere(Vec3 position)
{
	this->Position = position;
	this->Radius = 0;
}

bool Sphere::Test(const Sphere& s)
{
	return Test(*this, s);
}

bool Sphere::Test(const Sphere& s1, const Sphere& s2)
{
	float dist = (s2.Position - s1.Position).Length();

	return dist < s1.Radius+s2.Radius;
}
