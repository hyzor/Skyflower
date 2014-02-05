#ifndef PHYSICS_PHYSICSENGINEIMPL_H
#define PHYSICS_PHYSICSENGINEIMPL_H

#include "shared/platform.h"
#include "PhysicsEntityImpl.h"
#include "PhysicsEngine.h"
#include <vector>

/*
	A handle from which to create and destroy physicsentities
*/
class PhysicsEngineImpl : public PhysicsEngine
{
private:
	std::vector<PhysicsEntity*> mPhysicsEntities;
public:
	PhysicsEngineImpl();
	PhysicsEngineImpl(const PhysicsEngineImpl& other);
	~PhysicsEngineImpl();

	void Update(float deltaTime);

	PhysicsEntity* CreateEntity();
	PhysicsEntity* CreateEntity(Vec3 pos);
	void DestroyEntity(PhysicsEntity* physEntity);

	//Set the global (static) gravity kept within the physicsentities
	void SetGlobalGravity(Vec3 gravity);
};

#endif