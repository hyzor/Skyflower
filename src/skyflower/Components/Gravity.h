#ifndef COMPONENT_GRAVITY_H
#define COMPONENT_GRAVITY_H

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

class GravityComponent : public Component {

public:
	GravityComponent() : Component("Gravity")
	{ 
		this->p = new Physics();
	};

	virtual ~GravityComponent() { delete this->p; };

	Physics* getPhysicsInstance() { return this->p; };

	// we are added to an Entity, and thus to the component system
	void addedToEntity() {
		cout << "A gravity component was added to the system." << endl;
	}

	void update(float deltaTime)
	{
		Vec3 pos = getEntityPos();
		p->update(deltaTime);
		this->p->addGravityCalc(pos);

		updateEntityPos(pos);
	}
private:
	Physics* p;
};

#endif