#ifndef COMPONENT_PROJECTILE_H
#define COMPONENT_PROJECTILE_H

#include "Cistron.h"
#include <string>
#include <iostream>
#include <sstream>
#include "shared/Vec3.h"
#include "physics/Physics.h"
#include "physics/Collision.h"
#include "Entity.h"
using namespace std;
using namespace Cistron;

class Projectile : public Component {

public:

	Projectile() : Component("Projectile")
	{ 
		this->projectileDirection = Vec3(0.0f, DEFAULT_THROW_Y_AXIS, 0.0f);
	}
	virtual ~Projectile()
	{
	}

	// we are added to an Entity, and thus to the component system
	void addedToEntity() {
		this->p = getOwner()->getPhysics();

		requestMessage("FireProjectile", &Projectile::FireProjectile);
		requestMessage("FireProjectile", &Projectile::FireProjectileAt);
		requestMessage("StopProjectile", &Projectile::StopProjectile);
	}

	void removeFromEntity()
	{
		this->p = NULL;
	}

	void update(float deltaTime)
	{
		Vec3 pos = getEntityPos();
		Vec3 rot = getEntityRot();

		//std::vector<CollisionInstance*> instances = Collision::GetInstance()->GetCollisionInstances();
		//Ray r = Ray(pos+Vec3(0,5,0), Vec3(0, -5, 0));
		//float col = 0;
		//for (size_t i = 0; i < instances.size(); i++)
		//{
		//	if (instances[i] != getEntityCollision())
		//	{
		//		float t = instances[i]->Test(r);
		//		if (t > 0)
		//		{
		//			col = t;
		//			break;
		//		}
		//	}
		//}
		//if (col) //om kollision flytta tillbaka
		//{
		//	pos.Y -= (1 - col)*r.Dir.Y;
		//	p->setVelocity(Vec3());
		//	p->setJumping(false);
		//	p->setFlying(false);
		//}
	}

	void sendAMessage(string message)
	{
		//cout << "hej det är jag som ropar på denna funktionen: " << this->fName << endl;
		sendMessage(message);
	}

private:
	Physics* p;
	Vec3 projectileDirection;

	void FireProjectile(Message const& msg)
	{
		Vec3 pos = getEntityPos();
		projectileDirection = p->getLook();
		projectileDirection.Y = DEFAULT_THROW_Y_AXIS;
		projectileDirection.X *= 200.0f;
		projectileDirection.Z *= 200.0f;
		if (p->throwEntity(pos, projectileDirection))
		{
			//play sound, similar to jumping in Moevement.h
			playProjectileSound();
		}

		updateEntityPos(pos);
	}

	void FireProjectileAt(Message const& msg)
	{
		Vec3 pos = getEntityPos();
		Vec3 target = Vec3(100.0f, 100.0f, 100.0f); //defined here for now, should be supplied from elsewhere. For example, AI might want to throw something towards the players position


		if (p->throwEntity(pos, projectileDirection))
		{
			//play sound, similar to jumping in Moevement.h
			playProjectileSound();
		}

		updateEntityPos(pos);
	}

	void StopProjectile(Message const& msg)
	{
		
	}

	void playProjectileSound()
	{
		Entity *owner = getOwner();

		if (owner)
		{
			float soundPosition[3] = { 0.0f, 0.0f, 0.0f };
			owner->getModules()->sound->PlaySound("player/wilhelm_scream.wav", soundPosition, 1.0f, true);
		}
	}
};

#endif