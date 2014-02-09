#ifndef RAY_H
#define RAY_H

#include "shared/platform.h"
#include "shared/Vec3.h"
#include "Box.h"

class DLL_API Ray
{
	public:

		Ray();
		Ray(float X, float Y, float Z, float dX, float dY, float dZ);
		Ray(Vec3 Pos, Vec3 Dir);

		void Set(const Vec3& pos, const Vec3& dir);
		const Vec3& GetPos();
		void SetPos(const Vec3& pos);
		const Vec3& GetDir();

		const Box& GetBox();
	private:

		void calcBox();

		Box rayBounds;

		Vec3 Pos;
		Vec3 Dir;
};


#endif