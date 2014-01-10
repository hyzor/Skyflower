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

	// constructor - age is fixed at creation time
	AI() : Component("AI")
	{ 
	};
	virtual ~AI() {};




	// we are added to an Entity, and thus to the component system
	void addedToEntity() {
		cout << "A AI was added to the system." << endl;

		requestMessage("MoveToTarget", &AI::MoveToTarget);
	}

	void sendAMessage(string message)
	{
		//cout << "hej det är jag som ropar på denna funktionen: " << this->fName << endl;
		sendMessage(message);
	}
private:

	void MoveToTarget(Message const& msg)
	{
		//sendMessageToEntity(this->getOwnerId(), "StartMoveForward");
		/*Vec3 pos = getEntityPos();
		//pos.Y -= 0.01f;

		std::vector<CollisionInstance*> instances = Collision::GetInstance()->GetCollisionInstances();
		Ray r = Ray(pos+Vec3(0,5,0), Vec3(0, -5, 0));
		float col = 0;
		for (size_t i = 0; i < instances.size(); i++)
		{
			if (instances[i] != getEntityCollision())
			{
				float t = instances[i]->Test(r);
				if (t > 0)
				{
					col = t;
					break;
				}
			}
		}
		if (col) //om kollision flytta tillbaka
		{
			pos.Y -= (1 - col)*r.Dir.Y;
		}

		updateEntityPos(pos);*/
	}



};

#endif