#ifndef FIELD_H
#define FIELD_H

#include "shared\Vec3.h"

class Field
{
	public:
		Field(float Weight, float Size, Vec3 Pos);

		void Move(Vec3 Pos);
		void ChangeWeight(float Weight);

		float GetWeight(Vec3 pos);
	private:
		float Weight;
		float Size;
		Vec3 Pos;

};



#endif