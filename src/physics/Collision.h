#ifndef COLLISION_H
#define COLLISION_H

#include "Box.h"
#include "Sphere.h"
#include "Triangle.h"
#include "CollisionInstance.h"
#include <map>

class DLL_API Collision
{
	public:
		Collision();
		~Collision();

		CollisionInstance* CreateCollisionInstance(std::string file, Vec3 Position);
		void DeleteCollisionInstance(CollisionInstance* ci);

		std::vector<CollisionInstance*> GetCollisionInstances();

	private:
		std::map<std::string, CollisionModel*> models;
		std::vector<CollisionInstance*> instances;
};

#endif
