#ifndef PUSH_H
#define PUSH_H

#include <cstdio>
#include <string>

#include "shared/Vec3.h"
#include "Cistron.h"
#include "Entity.h"
#include "Movement.h"

using namespace std;
using namespace Cistron;

class Push : public Component {

public:
	Push() : Component("Push")
	{
	}

	virtual ~Push()
	{
	}

	void addedToEntity()
	{
		//requestMessage("Wall", &Push::wall);
	}

	void removeFromEntity()
	{
	}

	void update(float deltaTime)
	{
		Entity *pusher = getOwner();
		Entity *pushedObject = pusher->wall;

		if (pushedObject != nullptr)
		{
			if (pushedObject->hasComponents("Box"))
			{
				Vec3 pushedObjectPos = pushedObject->returnPos();
				Vec3 dir = pushedObjectPos - pusher->returnPos();
				Vec3 rotation = Vec3(0.0f, 0.0f, 0.0f);

				if (abs(dir.X) > abs(dir.Z))
				{
					float sign = copysign(1.0f, dir.X);
					dir = Vec3(sign, 0.0f, 0.0f);
					rotation.Y = DegreesToRadians(-90.0f * sign);
				}
				else
				{
					float sign = copysign(1.0f, dir.Z);
					dir = Vec3(0.0f, 0.0f, sign);
					rotation.Y = DegreesToRadians(90.0f + 90.0f * sign);
				}
				
				pushedObject->updatePos(pushedObjectPos + dir * deltaTime * pushedObject->getComponent<Movement*>("Movement")->GetSpeed());
				pusher->updateRot(rotation);
			}
		}
	}

private:

	/*void wall(Message const& msg)
	{
		if (getOwner()->wall->hasComponents("Box"))
		{
			int i = 0;
		}
	}*/


};

#endif