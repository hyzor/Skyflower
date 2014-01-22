#ifndef FIELD_H
#define FIELD_H

#include "shared\Vec3.h"
#include "physics\Box.h"

class Field
{
	public:
		Field(float Weight, float Size, Vec3 Pos, Box safe);

		void Move(Vec3 Pos);
		void ChangeWeight(float Weight);

		float GetWeight(Vec3 pos);

		Box GetSafe();

		bool Active;
	private:
		float Weight;
		float Size;
		Vec3 Pos;
		Box Safe;
};



#endif