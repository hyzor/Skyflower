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
	//push only selected entity
	void push(Entity* target);
	//push all entites in range
	void pushAll();

	bool isPushingBox();
	bool isDraging();
	bool isResettingSpeed();
	bool colliding(Entity* target);

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
	bool resetSpeed;

	float pSpeed;
	float currSpeed;

	Entity* box;
	Vec3 relativePos;

};

#endif