#include "Sphere.h"

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

bool Sphere::Test(Sphere s)
{
	return Test(*this, s);
}

bool Sphere::Test(Sphere s1, Sphere s2)
{
	float distX = (s2.Position.X - s1.Position.X);
	float distY = (s2.Position.Y - s1.Position.Y);
	float distZ = (s2.Position.Z - s1.Position.Z);
	float dist = (float)sqrt(distX*distX + distY*distY + distZ*distZ);

	return dist < s1.Radius+s2.Radius;
}