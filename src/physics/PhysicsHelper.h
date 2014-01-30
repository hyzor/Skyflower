#ifndef PHYSICS_PHYSICSHELPER_H
#define PHYSICS_PHYSICSHELPER_H

#include "shared/Vec3.h"

#define GRAVITY_LOW Vec3(0.0f, -60.82f, 0.0f)
#define GRAVITY_DEFAULT Vec3(0.0f, -120.82f, 0.0f)
#define GRAVITY_HIGH Vec3(0.0f, -180.82f, 0.0f)

#define MASS_LOW 25.0f
#define MASS_DEFAULT 50.0f
#define MASS_HIGH 75.0f

#define VELOCITY_DEFAULT Vec3(0.0f, 0.0f, 0.0f)

#define MOVEMENTSPEED_LOW 25.0f
#define MOVEMENTSPEED_DEFAULT 50.0f
#define MOVEMENTSPEED_HIGH 75.0f

#define JUMP_VELOCITY_LOW 15.0f
#define JUMP_VELOCITY_DEFAULT 100.0f//25.0f
#define JUMP_VELOCITY_HIGH 35.0f

//#define PROJECTILE_TIME_LOW 0.75f
//#define PROJECTILE_TIME_DEFAULT 1.5f
//#define PROJECTILE_TIME_HIGH 2.5f
#define PROJECTILE_HEIGHT_DEFAULT 5
#define PROJECTILE_HEIGHT_HIGH 10.0f
#define PROJECTILE_HEIGHT_LOW 2.5f
#define PROJECTILE_LENGTH_SHORT 40.0f
#define PROJECTILE_LENGTH_DEFAULT 50.0f
#define PROJECTILE_LENGTH_FAR 150.0f
#define PROJECTILE_ANGLE_DEGREES_DEFAULT 30.0f

namespace PhysicsHelper
{
	const float PI = 3.141592653589f;


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