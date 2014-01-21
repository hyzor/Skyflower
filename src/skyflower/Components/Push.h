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
		if (getOwner()->wall != nullptr)
		{
			if (getOwner()->wall->hasComponents("Box"))
			{
				Vec3 dir = getOwner()->returnPos()-getOwner()->wall->returnPos();

				Vec3 n1 = dir*Vec3(1, 0, 0);
				Vec3 n3 = dir*Vec3(0, 0, 1);

				if (n1.Length() > n3.Length())
					dir = n1.Normalize();
				else
					dir = n3.Normalize();


				getOwner()->wall->updatePos(getOwner()->wall->returnPos() - dir*deltaTime*getOwner()->wall->getComponent<Movement*>("Movement")->GetSpeed());
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