#define _USE_MATH_DEFINES
#include <cmath>

#include "Sound/SoundEngine.h"
#include "shared/util.h"

#include "Components/BoxComp.h"
#include "Components/Gravity.h"
#include "EntityManager.h"

// Must be included last!
#include "shared/debug.h"

static const char *pushBoxSounds[] = {
	"box/move1.wav",
	"box/move2.wav",
	"box/move3.wav",
	"box/move4.wav",
	"box/move5.wav"
};

BoxComp::BoxComp(float speed, float minDist) : Component("Box")
{
	this->speed = speed;
	pNormal = Vec3(0, 1, 0);
	rotx = 0;
	rotz = 0;
	this->minDist = minDist;
	this->canBeMoved = true;
}

BoxComp::~BoxComp()
{
}

void BoxComp::addedToEntity()
{
	requestMessage("Respawn", &BoxComp::respawn);
	requestMessage("CanBeMoved", &BoxComp::setCanBeMoved);
	requestMessage("CanNotBeMoved", &BoxComp::setCanNotBeMoved);

	this->m_oldPosition = getOwner()->returnPos();
	this->m_oldPosition.Y = 0.0f;
	this->m_distanceTraveled = 0.0f;
	startPos = getOwner()->returnPos()*Vec3(1, 0, 1);
}

void BoxComp::removeFromEntity()
{
}

void BoxComp::update(float dt)
{
	GravityComponent* gc = getOwner()->getComponent<GravityComponent*>("Gravity");
	if (gc)
	{
		//rotate box
		const Vec3 *groundNormal = gc->GetGroundNormal();
		Vec3 normal;

		if (groundNormal)
		{
			normal = *groundNormal;
			pNormal = normal;
		}
		else
			normal = pNormal;


		if (getOwner()->ground)
		{
			fall = fallDir();

			Vec3 originalPosition = getOwner()->returnPos();
			Vec3 position = Vec3(originalPosition.X, 0.0f, originalPosition.Z);

			m_distanceTraveled += (position - m_oldPosition).Length();
			m_oldPosition = position;

			if (m_distanceTraveled >= 10.0f)
			{
				m_distanceTraveled = 0.0f;

				getOwner()->getModules()->sound->PlaySound(pushBoxSounds[rand() % ARRAY_SIZE(pushBoxSounds)], 1.0f, &originalPosition.X);
			}
		}
		//box is rotating over the edge
		if (isFalling())
		{
			rotz += fall.X*(float)M_PI*dt;
			rotx += fall.Z*(float)M_PI*dt;

			getOwner()->updatePos(getOwner()->returnPos() + fall * 10 * dt);
		}
		//the box is still on the ground, not rotating over an edge.
		else
		{
			//r�kna rotation l�ngs med vectorn
			if(normal.Z < 0)
				rotx = asinf(Vec3(0.0f, normal.Y, normal.Z).Normalize().Y) - (float)M_PI_2;
			else
				rotx = -asinf(Vec3(0.0f, normal.Y, normal.Z).Normalize().Y) + (float)M_PI / 2;
			if(normal.X < 0)
				rotz = -asinf(Vec3(normal.X, normal.Y, 0.0f).Normalize().Y) + (float)M_PI_2;
			else
				rotz = asinf(Vec3(normal.X, normal.Y, 0.0f).Normalize().Y) - (float)M_PI / 2;
		}

		getOwner()->updateRot(Vec3(rotx, getOwner()->returnRot().Y, rotz));
	}
}

float BoxComp::GetSpeed()
{
	return speed;
}


Vec3 BoxComp::fallDir()
{
	GravityComponent* gravity = getOwner()->getComponent<GravityComponent*>("Gravity");
	if (gravity)
	{
		Vec3 offset;
		int count = 0;
		for (int i = 0; i < gravity->NrOfGroundRays(); i++)
		{
			if (!gravity->GroundRayColliding(i))
			{
				count++;
				offset += Vec3(1, 0, 1) * gravity->GroundRay(i).GetPos();
			}
		}
		offset /= gravity->NrOfGroundRays();
		if ((offset.Length() > 0.1f && count > 3))
			return offset.Normalize();
	}
	return Vec3();
}

void BoxComp::respawn(Message const& msg)
{
	fall = Vec3();
}

bool BoxComp::isFalling()
{
	return fall != Vec3();
}

float BoxComp::MinDist()
{
	return minDist;
}

bool BoxComp::getCanBeMoved()
{
	return this->canBeMoved;
}

void BoxComp::setCanBeMoved(Message const &msg)
{
	this->canBeMoved = true;
}

void BoxComp::setCanNotBeMoved(Message const &msg)
{
	this->canBeMoved = false;
}