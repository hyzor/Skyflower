#ifndef PUSH_H
#define PUSH_H

#include <cstdio>
#include <string>

#include "shared/Vec3.h"
#include "Cistron.h"
#include "Entity.h"
#include "Movement.h"
#include "Messenger.h"

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
		//requestMessage("inAir", &Push::stopPush);
		requestMessage("Wall", &Push::stopPush);
	}

	void removeFromEntity()
	{
	}

	void update(float deltaTime);

	bool canPush(Entity* target);

	//push only selected entity
	void push(Entity* target);
	//push all entites in range
	void pushAll();


private:


	void stopPush(Message const& msg);


};

#endif