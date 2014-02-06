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
		BoxComp(float speed) : Component("Box")
		{
			this->speed = speed;
		}

		virtual ~BoxComp()
		{
		}

		void addedToEntity()
		{

		}

		void removeFromEntity()
		{
		}

		void update(float deltaTime);

		float GetSpeed()
		{
			return speed;
		}
	private:

		float speed;

		Triangle getGroundTriangle(Entity* e, Entity* ground);
};


#endif