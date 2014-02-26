#include "ParticleSystemComp.h"

// Must be included last!
#include "shared/debug.h"

void ParticleSystemComp::addedToEntity()
{
	this->mParticleSystem = getOwner()->getModules()->graphics->CreateParticleSystem();
	this->mParticleSystem->SetActive(false);
}

void ParticleSystemComp::removeFromEntity()
{
	getOwner()->getModules()->graphics->DeleteParticleSystem(this->mParticleSystem);
	this->mParticleSystem = NULL;
}

void ParticleSystemComp::update(float dt)
{

}