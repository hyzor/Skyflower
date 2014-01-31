#include "PhysicsEngineImpl.h"

// Must be included last!
#include "shared/debug.h"

DLL_API PhysicsEngine* CreatePhysicsEngine()
{
	PhysicsEngineImpl* p = new PhysicsEngineImpl();

	return (PhysicsEngine*)p;
}

DLL_API void DestroyPhysicsEngine(PhysicsEngine* pEngine)
{
	if (!pEngine)
		return;

	PhysicsEngineImpl *pEngineImpl = (PhysicsEngineImpl*)pEngine;

	delete pEngineImpl;
}

PhysicsEngineImpl::PhysicsEngineImpl()
{
	PhysicsEntityImpl::SetGlobalGravity(GRAVITY_DEFAULT);
}

PhysicsEngineImpl::PhysicsEngineImpl(const PhysicsEngineImpl& other)
{

}

PhysicsEngineImpl::~PhysicsEngineImpl()
{
	for (unsigned int i = 0; i < this->mPhysicsEntities.size(); i++)
	{
		if (this->mPhysicsEntities.at(i) != nullptr)
		{
			delete this->mPhysicsEntities.at(i);
		}
	}
	this->mPhysicsEntities.clear();
}

PhysicsEntity* PhysicsEngineImpl::CreateEntity()
{
	PhysicsEntity* physEntity = (PhysicsEntity*)new PhysicsEntityImpl();
	this->mPhysicsEntities.push_back(physEntity);

	return physEntity;
}

PhysicsEntity* PhysicsEngineImpl::CreateEntity(Vec3 pos)
{
	PhysicsEntity* physEntity = (PhysicsEntity*)new PhysicsEntityImpl(pos);
	this->mPhysicsEntities.push_back(physEntity);

	return physEntity;
}

void PhysicsEngineImpl::DestroyEntity(PhysicsEntity* physEntity)
{
	for (unsigned int i = 0; i < this->mPhysicsEntities.size(); i++)
	{
		if (physEntity == this->mPhysicsEntities.at(i))
		{
			delete physEntity;
			this->mPhysicsEntities.erase(this->mPhysicsEntities.begin() + i);
			return;
		}
	}
}

void PhysicsEngineImpl::SetGlobalGravity(Vec3 gravity)
{
	PhysicsEntityImpl::SetGlobalGravity(gravity);
}