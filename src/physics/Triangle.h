#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "shared/platform.h"
#include "Vec3.h"
#include "Ray.h"

class DLL_API Triangle
{
	public:
		Vec3 P1;
		Vec3 P2;
		Vec3 P3;

		Triangle();
		Triangle(Vec3 P1, Vec3 P2, Vec3 P3);

		float Test(Ray &r);
		static float Test(Triangle &t, Ray &r);

	private:
};



#endif