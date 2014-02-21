#ifndef BOXCOMP
#define BOXCOMP

#include "Cistron.h"
#include <string>
#include <iostream>
#include <sstream>
#include "shared/Vec3.h"
#include "physics/PhysicsEngine.h"
#include "physics/Collision.h"
#include "Entity.h"
using namespace std;
using namespace Cistron;

class BoxComp : public Component
{
public:
	BoxComp(float speed);
	virtual ~BoxComp();

	void addedToEntity();
	void removeFromEntity();

	void update(float deltaTime);

	float GetSpeed();

	bool isFalling();

private:
	float speed;
	Vec3 pNormal;

	Vec3 m_oldPosition;
	float m_distanceTraveled;

	Vec3 fallDir();
	void respawn(Message const& msg);

	Vec3 fall;
	float rotx;
	float rotz;
};

#endif
