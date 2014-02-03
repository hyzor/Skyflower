#ifndef COLLISION_H
#define COLLISION_H

#include <string>
#include <vector>

#include "shared/platform.h"
#include "shared/Vec3.h"

#include "CollisionInstance.h"

class Collision
{
public:
	virtual ~Collision() {}

	virtual CollisionInstance* CreateCollisionInstance(const std::string &file, Vec3 Position) = 0;
	virtual void DeleteCollisionInstance(CollisionInstance* ci) = 0;

	virtual const std::vector<CollisionInstance*> &GetCollisionInstances() = 0;
};

DLL_API Collision *CreateCollision(const std::string &relativePath);
DLL_API void DestroyCollision(Collision *collision);

#endif
