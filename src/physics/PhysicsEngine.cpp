#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine()
{
	PhysicsEntity::SetGlobalGravity(DEFAULT_GRAVITY);
}

PhysicsEngine::PhysicsEngine(const PhysicsEngine& other)
{

}

PhysicsEngine::~PhysicsEngine()
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

PhysicsEntity* PhysicsEngine::CreateEntity(PhysicsEntity* physEntity)
{
	physEntity = new PhysicsEntity();
	this->mPhysicsEntities.push_back(physEntity);

	return physEntity;
}

PhysicsEntity* PhysicsEngine::CreateEntity(PhysicsEntity* physEntity, Vec3 pos)
{
	physEntity = new PhysicsEntity(pos);
	this->mPhysicsEntities.push_back(physEntity);

	return physEntity;
}

void PhysicsEngine::DestroyEntity(PhysicsEntity* physEntity)
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

void PhysicsEngine::SetGlobalGravity(Vec3 gravity)
{
	PhysicsEntity::SetGlobalGravity(gravity);
}