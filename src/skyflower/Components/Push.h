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


private:
	void stopPush(Message const& msg);
	void setCanPush(Message const& msg);
	void setCanNotPush(Message const& msg);
	void beingPushed(Message const& msg);

private:
	bool m_isPushingBox;
	Vec3 m_initialPushDirection;
	bool canPush;
	bool isColliding;
};

#endif