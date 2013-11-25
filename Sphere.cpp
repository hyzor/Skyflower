#include "Sphere.h"

Sphere::Sphere()
{
	this->X = 0;
	this->Y = 0;
	this->Z = 0;
	this->Radius = 0;
}

Sphere::Sphere(float X, float Y, float Z)
{
	this->X = X;
	this->Y = Y;
	this->Z = Z;
	this->Radius = 0;
}

Sphere::Sphere(float X, float Y, float Z, float Radius)
{
	this->X = X;
	this->Y = Y;
	this->Z = Z;
	this->Radius = Radius;
}

bool Sphere::Test(Sphere s)
{
	return Test(*this, s);
}

bool Sphere::Test(Sphere s1, Sphere s2)
{
	float distX = (s2.X - s1.X);
	float distY = (s2.Y - s1.Y);
	float distZ = (s2.Z - s1.Z);
	float dist = (float)sqrt(distX*distX + distY*distY + distZ*distZ);

	return dist < s1.Radius+s2.Radius;
}