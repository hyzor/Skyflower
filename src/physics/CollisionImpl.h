#ifndef COLLISIONIMPL_H
#define COLLISIONIMPL_H

#include <map>

#include "Collision.h"
#include "CollisionModel.h"
#include "CollisionInstanceImpl.h"

class CollisionImpl : public Collision
{
	public:
		CollisionImpl(const std::string &relativePath);
		~CollisionImpl();

		CollisionInstance* CreateCollisionInstance(const std::string &file, Vec3 Position);
		void DeleteCollisionInstance(CollisionInstance* ci);

		const std::vector<CollisionInstance*> &GetCollisionInstances();

	private:
		std::string relativePath;

		std::map<std::string, CollisionModel*> models;
		std::vector<CollisionInstance*> instances;
};

#endif
