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

class DLL_API CollisionModel
{
	public:
		CollisionModel();
		~CollisionModel();

		void LoadObj(std::string file);

		Box GetBox();
		Triangle* GetTriangle(int index);
		ModelTreeParent* GetTree();
		int Triangles();
		
	private:
		std::vector<Triangle> triangles;
		Box bounds;

		std::vector<std::string> split(std::string line);
		ModelTreeParent* tree;
};


#endif