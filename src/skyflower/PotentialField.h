#ifndef POTENTIALFIELD_H
#define POTENTIALFIELD_H

#include <vector>
#include <stdlib.h>
#include <time.h>
#include "Field.h"

class PotentialField
{
	public:
		PotentialField();
		~PotentialField();

		Field* CreateField(float weight, float size, Vec3 pos);
		void DeleteField(Field* f);

		Vec3 GetDir(Vec3 pos);
		float GetWeight(Vec3 pos);
	private:

		std::vector<Field*> fields;

};





#endif