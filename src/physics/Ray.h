#ifndef RAY_H
#define RAY_H

#include "shared/platform.h"
#include "Vec3.h"

class DLL_API Ray
{
	public:
		Vec3 Pos;
		Vec3 Dir;

		Ray();
		Ray(float X, float Y, float Z, float dX, float dY, float dZ);
		Ray(Vec3 Pos, Vec3 Dir);
	private:
};


#endif