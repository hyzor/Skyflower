#include "Field.h"


Field::Field(float Weight, float Size, Vec3 Pos)
{
	this->Weight = Weight;
	this->Pos = Pos;
	this->Size = Size;
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
	float dist = (Pos - pos).Length();
	if (dist > Size)
		return 0;
	float val = (Size-dist)/Size*Weight;
	//if ((Weight > 0 && val < 0) || (Weight < 0 && val > 0))
		//val = 0;
	return val;
}