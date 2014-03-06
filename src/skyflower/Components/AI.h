#ifndef AI_H
#define AI_H

#include "Cistron.h"
#include <string>
#include <iostream>
#include <sstream>
#include "shared/Vec3.h"
#include "physics/Collision.h"
#include "PotentialField.h"
#include "Entity.h"

using namespace std;
using namespace Cistron;

class AI : public Component {

public:

	AI() : Component("AI")
	{
		this->canMove = true;
		centerradius = 0;
		attacktime = 0;
		nextattack = 10;
		curDir = Vec3();
		unsafeIndex = 0;
		for (int i = 0; i < 5; i++)
			unsafe[i] = nullptr;
	};
	virtual ~AI() 
	{
	};

	void addedToEntity();
	void update(float dt);
	void setTarget(Entity* e, float radius);
	void sendAMessage(string message)	{
	}

private:
	Vec3 curDir;
	float centerradius;
	float attacktime;
	float nextattack;
	bool canMove;

	int unsafeIndex;
	Field* unsafe[5];
	
	Entity* target;

	void collision(float dt);

	void stopMoving(Message const& msg)
	{
		this->canMove = false;
	}

	void startMoving(Message const& msg)
	{
		this->canMove = true;
	}
};

#endif