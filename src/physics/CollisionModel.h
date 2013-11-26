#ifndef COLLISIONMODEL_H
#define COLLISIONMODEL_H

#include "shared/platform.h"
#include "Vec3.h"
#include <string>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include "Triangle.h"
#include "ModelTree.h"
#include "Box.h"

class CollisionModel
{
	public:
		CollisionModel();
		~CollisionModel();

		void LoadObj(std::string file);

		Box GetBox();
		Triangle* GetTriangle(int index);
		int Triangles();
		
	private:
		std::vector<Triangle> triangles;
		Box bounds;

		std::vector<std::string> split(std::string line);
		ModelTreeParent* Tree;
};


#endif