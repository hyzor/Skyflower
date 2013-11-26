#include "Box.h"


Box::Box()
{
}

Box::Box(float X, float Y, float Z, float Width, float Height, float Depth)
{
	this->Position = Vec3(X, Y, Z);
	this->Size = Vec3(Width, Height, Depth);
}

Box::Box(Vec3 Position, Vec3 Size)
{
	this->Position = Position;
	this->Size = Size;
}

bool Box::Test(Box b)
{
	return Test(*this, b);
}

bool Box::Test(Box b1, Box b2)
{
	return (b1.Position.X + b1.Size.X > b2.Position.X && b1.Position.X < b2.Position.X + b2.Size.X) && (b1.Position.Y + b1.Size.Y > b2.Position.Y && b1.Position.Y < b2.Position.Y + b2.Size.Y) && (b1.Position.Z + b1.Size.Z > b2.Position.Z && b1.Position.Z < b2.Position.Z + b2.Size.Z);
}

Vec3 Box::Center()
{
	return Position + Size / 2;
}