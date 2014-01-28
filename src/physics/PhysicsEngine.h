#ifndef PHYSICS_PHYSICSENGINE_H
#define PHYSICS_PHYSICSENGINE_H

#include "shared/platform.h"
#include "PhysicsEntity.h"
#include <vector>

class DLL_API PhysicsEngine
{
private:
	std::vector<PhysicsEntity*> mPhysicsEntities;
public:
	PhysicsEngine();
	PhysicsEngine(const PhysicsEngine& other);
	~PhysicsEngine();

	PhysicsEntity* CreateEntity(PhysicsEntity* physEntity);
	PhysicsEntity* CreateEntity(PhysicsEntity* physEntity, Vec3 pos);
	void DestroyEntity(PhysicsEntity* physEntity);

	void SetGlobalGravity(Vec3 gravity);
};

#endif