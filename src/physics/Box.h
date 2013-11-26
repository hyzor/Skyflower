#ifndef BOX_H
#define BOX_H

#include "shared/platform.h"
#include "Vec3.h"

class DLL_API Box
{
	public:
		Vec3 Position;
		Vec3 Size;
		float X;
		float Y;
		float Z;
		float Width;
		float Height;
		float Depth;

		Box();
		Box(float X, float Y, float Z, float Width, float Height, float Depth);
		Box(Vec3 Position, Vec3 Size);

		bool Test(Box b);
		static bool Test(Box b1, Box b2);

		Vec3 Center();

	private:

};


#endif