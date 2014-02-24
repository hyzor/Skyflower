#ifndef SPHERE_h
#define SPHERE_H

#include "shared/platform.h"
#include <math.h>
#include "shared/Vec3.h"

class DLL_API Sphere
{
	public:
		Vec3 Position;
		float Radius;

		Sphere();
		Sphere(float X, float Y, float Z);
		Sphere(float X, float Y, float Z, float Radius);
		Sphere(Vec3 pos, float radius);
		Sphere(Vec3 pos);

		bool Test(const Sphere& s);
		static bool Test(const Sphere& s1, const Sphere& s2);

	private:
};



#endif