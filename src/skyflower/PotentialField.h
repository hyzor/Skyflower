#ifndef POTENTIALFIELD_H
#define POTENTIALFIELD_H

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include "Field.h"
#include "Physics/Triangle.h"
#include "Physics/Collision.h"
#include "Physics/Box.h"

class PotentialField
{
	public:
		PotentialField();
		~PotentialField();

		Field* CreateField(float weight, float size, Vec3 pos);
		Field* CreateField(std::string file, Vec3 pos, Vec3 scale);
		void DeleteField(Field* f);

		Vec3 GetDir(Vec3 pos);
		float GetWeight(Vec3 pos);
	private:

		std::vector<Field*> fields;

		std::vector<std::string> PotentialField::split(std::string line);

};





#endif