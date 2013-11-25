#include "Box.h"


Box::Box()
{
	this->X = 0;
	this->Y = 0;
	this->Z = 0;
	this->Width = 0;
	this->Height = 0;
	this->Depth = 0;
}

Box::Box(float X, float Y, float Z, float Width, float Height, float Depth)
{
	this->X = X;
	this->Y = Y;
	this->Z = Z;
	this->Width = Width;
	this->Height = Height;
	this->Depth = Depth;
}

bool Box::Test(Box b)
{
	return Test(*this, b);
}

bool Box::Test(Box b1, Box b2)
{
	return (b1.X + b1.Width > b2.X && b1.X < b2.X + b2.Width) && (b1.Y + b1.Height > b2.Y && b1.Y < b2.Y + b2.Height) && (b1.Z + b1.Depth > b2.Z && b1.Z < b2.Z + b2.Depth);
}