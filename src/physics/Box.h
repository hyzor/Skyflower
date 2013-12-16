#ifndef BOX_H
#define BOX_H

#include "shared/platform.h"
#include "shared/Vec3.h"

class DLL_API Box
{
	public:
		Vec3 Position;
		Vec3 Size;

		Box();
		Box(float X, float Y, float Z, float Width, float Height, float Depth);
		Box(Vec3 Position, Vec3 Size);

		bool Test(Box b);
		bool Test(Vec3 b);
		static bool Test(Box b1, Box b2);
		static bool Test(Box b1, Vec3 v);

		Vec3 Center();

	private:

};


#endif