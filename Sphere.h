#ifndef SPHERE_h
#define SPHERE_H

#include "shared/platform.h"
#include <math.h>

class DLL_API Sphere
{
	public:
		float X;
		float Y;
		float Z;
		float Radius;

		Sphere();
		Sphere(float X, float Y, float Z);
		Sphere(float X, float Y, float Z, float Radius);

		bool Test(Sphere s);
		static bool Test(Sphere s1, Sphere s2);

	private:
};



#endif