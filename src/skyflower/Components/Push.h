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
	Push();
	virtual ~Push();

	void addedToEntity();
	void removeFromEntity();

	void update(float deltaTime);

	bool colliding(Entity* target);

	//push only selected entity
	void push(Entity* target);
	//push all entites in range
	void pushAll();

	bool isPushingBox();

	bool isDraging();

private:
	void stopPush(Message const& msg);
	void beingPushed(Message const& msg);
	void pickUpStart(Message const& msg);
	void pickUpStop(Message const& msg);

private:
	bool m_isPushingBox;
	bool canPush;
	bool isColliding;
	bool canDrag;

	float pSpeed;
	float resetSpeed;


	Entity* box;
	Vec3 relativePos;

};

#endif