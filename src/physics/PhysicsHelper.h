#ifndef PHYSICS_PHYSICSHELPER_H
#define PHYSICS_PHYSICSHELPER_H

#include "shared/Vec3.h"

#define DEFAULT_GRAVITY Vec3(0.0f, -120.82f, 0.0f)
#define DEFAULT_MASS 50.0f //In KG
#define DEFAULT_VELOCITY Vec3(0.0f, 0.0f, 0.0f)
#define DEFAULT_MOVEMENTSPEED 50.0f
#define DEFAULT_JUMP_VELOCITY 35.0f
#define DEFAULT_THROW_TIME 2.5f
#define DEFAULT_THROW_ANGLE_DEGREES 30.0f
#define DEFAULT_THROW_HEIGHT 15.0f
#define THROW_HEIGHT_HIGH 30.0f
#define THROW_HEIGHT_LOW 5.0f

namespace PhysicsHelper
{
	inline float ToRadians(float degrees)
	{
		return (degrees * PI / 180.0f);
	}

	inline float ToDegrees(float radians)
	{
		return (radians * 180.0f / PI);
	}

	inline Vec3 ToRadians(Vec3 degrees)
	{
		return Vec3(ToRadians(degrees.X), ToRadians(degrees.Y), ToRadians(degrees.Z));
	}

	inline Vec3 ToDegrees(Vec3 radians)
	{
		return Vec3(ToDegrees(radians.X), ToDegrees(radians.Y), ToDegrees(radians.Z));
	}
}





#endif