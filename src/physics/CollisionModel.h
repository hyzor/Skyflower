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
#include "Sphere.h"

class CollisionModel
{
	public:
		CollisionModel();
		~CollisionModel();

		void Load(std::string file);

		Box GetBox();
		Sphere GetSphere();
		Triangle* GetTriangle(int index);
		ModelTreeParent* GetTree();
		int Triangles();
		
	private:
		std::vector<Triangle> triangles;
		Box bounds;
		Sphere sphere;

		std::vector<std::string> split(std::string line);
		ModelTreeParent* tree;


		void SaveTree(std::string file);
		bool LoadTree(std::string file);
};


#endif