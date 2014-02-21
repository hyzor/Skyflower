#define _USE_MATH_DEFINES
#include <cmath>

#include "Sound/SoundEngine.h"
#include "shared/util.h"

#include "Components/BoxComp.h"
#include "Components/Gravity.h"
#include "EntityManager.h"
#include "Application.h"

// Must be included last!
#include "shared/debug.h"

static const char *pushBoxSounds[] = {
	"player/box_move_1.wav",
	"player/box_move_2.wav",
	"player/box_move_3.wav",
	"player/box_move_4.wav",
	"player/box_move_5.wav"
};

BoxComp::BoxComp(float speed) : Component("Box")
{
	this->speed = speed;
	pNormal = Vec3(0, 1, 0);
	rotx = 0;
	rotz = 0;
}

BoxComp::~BoxComp()
{
}

void BoxComp::addedToEntity()
{
	requestMessage("Respawn", &BoxComp::respawn);
	this->m_oldPosition = getEntityPos();
	this->m_oldPosition.Y = 0.0f;
	this->m_distanceTraveled = 0.0f;
}

void BoxComp::removeFromEntity()
{
}

void BoxComp::update(float dt)
{
	//rotate box
	const Vec3 *groundNormal = getOwner()->getComponent<GravityComponent*>("Gravity")->GetGroundNormal();
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

		Vec3 originalPosition = getEntityPos();
		Vec3 position = Vec3(originalPosition.X, 0.0f, originalPosition.Z);

		m_distanceTraveled += (position - m_oldPosition).Length();
		m_oldPosition = position;

		if (m_distanceTraveled >= 10.0f)
		{
			m_distanceTraveled = 0.0f;

			// FIXME: Project the sound's position onto the ground.
			getOwner()->getModules()->sound->PlaySound(GetPlayerSoundFile(pushBoxSounds[rand() % ARRAY_SIZE(pushBoxSounds)]), 1.0f, &originalPosition.X);
		}
	}
	//box is rotating over the edge
	if (fall != Vec3())
	{
		rotz += fall.X*(float)M_PI*dt;
		rotx -= fall.Z*(float)M_PI*dt;

		getOwner()->updatePos(getOwner()->returnPos() + fall*10*dt);
	}
	//the box is still on the ground, not rotating over an edge.
	else
	{
		//räkna rotation längs med vectorn
		rotx = asinf(Vec3(0.0f, normal.Y, normal.Z).Normalize().Y);
		rotz = asinf(Vec3(normal.X, normal.Y, 0.0f).Normalize().Y);
	}

	getOwner()->updateRot(Vec3(M_PI*2 - (rotx - M_PI_2), getOwner()->returnRot().Y, -rotz + M_PI_2));
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