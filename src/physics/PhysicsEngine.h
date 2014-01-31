#ifndef PHYSICS_PHYSICSENGINE_H
#define PHYSICS_PHYSICSENGINE_H

#include "shared/platform.h"
#include "PhysicsEntity.h"

class PhysicsEngine
{
public:
	virtual ~PhysicsEngine(){};

	virtual PhysicsEntity* CreateEntity() = 0;
	virtual PhysicsEntity* CreateEntity(Vec3 pos) = 0;
	virtual void DestroyEntity(PhysicsEntity* physEntity) = 0;

	virtual void SetGlobalGravity(Vec3 gravity) = 0;
};

DLL_API PhysicsEngine* CreatePhysicsEngine();
DLL_API void DestroyPhysicsEngine(PhysicsEngine*);

#endif