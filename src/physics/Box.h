#ifndef BOX_H
#define BOX_H

#include "shared/platform.h"

class DLL_API Box
{
	public:
		float X;
		float Y;
		float Z;
		float Width;
		float Height;
		float Depth;

		Box();
		Box(float X, float Y, float Z, float Width, float Height, float Depth);

		bool Test(Box b);
		static bool Test(Box b1, Box b2);

	private:

};


#endif