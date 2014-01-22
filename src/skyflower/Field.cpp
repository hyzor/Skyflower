#include "Field.h"


Field::Field(float Weight, float Size, Vec3 Pos, Box safe)
{
	this->Weight = Weight;
	this->Pos = Pos;
	this->Size = Size;
	this->Safe = safe;
	this->Active = true;
}

void Field::ChangeWeight(float Weight)
{
	this->Weight = Weight;
}

void Field::Move(Vec3 Pos)
{
	this->Pos = Pos;
}

float Field::GetWeight(Vec3 pos)
{
	if (!Active)
		return 0;

	float dist = (Pos - pos).Length();

	if (dist > Size)
		return 0;

	float val;
	float x = dist / Size;
	float y = sqrtf(1 - x*x);

	val = y*Weight;
	
	//(1 - dist / Size)*Weight;
	//if ((Weight > 0 && val < 0) || (Weight < 0 && val > 0))
		//val = 0;
	//if(Weight > 0)
		//val = (Size / dist)*Weight;
	//else
		//val = (Size / dist)*Weight;
	return val;
}

Box Field::GetSafe()
{
	Box b = Safe;
	b.Position += Pos;
	return b;
}