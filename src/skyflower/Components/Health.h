#ifndef HEALTH_H
#define HEALTH_H

#include "Cistron.h"
#include "Entity.h"
#include <string>
#include <iostream>
#include <cassert>
using namespace std;
using namespace Cistron;

class Health : public Component {

public:

	// constructor - age is fixed at creation time
	Health(int maxHP) : Component("Health")
	{ 
		this->health = maxHP;
		this->maxHP = maxHP;
	};
	virtual ~Health() {};


	// we are added to an Entity, and thus to the component system
	void addedToEntity()
	{
		Entity *owner = getOwner();
		assert(owner);

		requestMessage("Respawn", &Health::Respawn);
	}

	void removeFromEntity()
	{
		Entity *owner = getOwner();
		assert(owner);	
	}

	bool isAlive()
	{
		return health > 0;
	}
	void setHealth(int health)
	{
		this->health = health;
	}
	int getHealth()
	{
		return this->health;
	}
private:
	int health;
	int maxHP;

	void Respawn(Message const& msg)
	{
		updateEntityPos(getOwner()->spawnpos);
		health = maxHP;
	}
};

#endif