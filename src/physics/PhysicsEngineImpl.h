#ifndef PHYSICS_PHYSICSENGINEIMPL_H
#define PHYSICS_PHYSICSENGINEIMPL_H

#include "shared/platform.h"
#include "PhysicsEntityImpl.h"
#include "PhysicsEngine.h"
#include <vector>

class PhysicsEngineImpl : public PhysicsEngine
{
private:
	std::vector<PhysicsEntity*> mPhysicsEntities;
public:
	PhysicsEngineImpl();
	PhysicsEngineImpl(const PhysicsEngineImpl& other);
	~PhysicsEngineImpl();

	PhysicsEntity* CreateEntity();
	PhysicsEntity* CreateEntity(Vec3 pos);
	void DestroyEntity(PhysicsEntity* physEntity);

	void SetGlobalGravity(Vec3 gravity);
};

#endif