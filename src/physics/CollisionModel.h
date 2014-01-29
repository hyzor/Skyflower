#ifndef COLLISIONMODEL_H
#define COLLISIONMODEL_H

#include "shared/Vec3.h"
#include <string>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <sstream>
#include "Triangle.h"
#include "ModelTree.h"
#include "Box.h"

class CollisionModel
{
	public:
		CollisionModel();
		~CollisionModel();

		void Load(std::string file);

		Box GetBox();
		Triangle* GetTriangle(int index);
		ModelTreeParent* GetTree();
		int Triangles();
		
	private:
		std::vector<Triangle> triangles;
		Box bounds;

		std::vector<std::string> split(std::string line);
		ModelTreeParent* tree;


		void SaveTree(std::string file);
		bool LoadTree(std::string file);
};


#endif