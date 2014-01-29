#include "PhysicsEntity.h"

Vec3 PhysicsEntity::mGlobalGravity = Vec3(0.0f, 0.0f, 0.0f);

PhysicsEntity::PhysicsEntity()
{
	this->mGravity = GRAVITY_DEFAULT;
	this->mMass = MASS_DEFAULT;
	this->mProjectileAngle = 0.0f;
	this->mDeltaTime = 0.0;
	this->mVelocity = VELOCITY_DEFAULT;
	this->mStates = PhysicsEntityStates();
}

PhysicsEntity::PhysicsEntity(Vec3 pos)
{
	this->mGravity = GRAVITY_DEFAULT;
	this->mMass = MASS_DEFAULT;
	this->mProjectileAngle = 0.0f;
	this->mDeltaTime = 0.0;
	this->mVelocity = VELOCITY_DEFAULT;
	this->mStates = PhysicsEntityStates();
}


PhysicsEntity::PhysicsEntity(const PhysicsEntity& other)
{
	this->mGravity = other.mGravity;
	this->mMass = other.mMass;
	this->mProjectileAngle = other.mProjectileAngle;
	this->mDeltaTime = 0.0;
	this->mVelocity = other.mVelocity;
	this->mStates = other.mStates;
}


PhysicsEntity::~PhysicsEntity(){}

void PhysicsEntity::SetGravity(Vec3 gravity)
{
	this->mGravity = gravity;
}

void PhysicsEntity::SetGlobalGravity(Vec3 gravity)
{
	PhysicsEntity::mGlobalGravity = gravity;
}

void PhysicsEntity::SetMass(float m)
{
	this->mMass = m;
}

float PhysicsEntity::GetMass() const
{
	return this->mMass;
}

Vec3 PhysicsEntity::GetGravity() const
{
	return this->mGravity;
}

Vec3 PhysicsEntity::GetVelocity() const
{
	return this->mVelocity;
}
void PhysicsEntity::Update(float dt)
{
	this->mDeltaTime = dt;
	this->mOrient.Update(dt);
}

void PhysicsEntity::AddGravityCalc(Vec3 &pos, Vec3 &velocity, bool addGravity)
{
	Vec3 previousVelocity = velocity;
	Vec3 previousPos = pos;

	velocity = previousVelocity;
	pos = previousPos + previousVelocity * this->mDeltaTime;

	if (addGravity)
	{
		velocity += this->mGravity * this->mDeltaTime;
		pos += this->mGravity * (this->mDeltaTime * this->mDeltaTime) / 2.0f;
	}
}

void PhysicsEntity::SetVelocity(Vec3 vel)
{
	this->mVelocity = vel;
}

void PhysicsEntity::AddGravityCalc(Vec3 &pos, bool addGravity)
{
	Vec3 previousPos = pos;
	Vec3 previousVelocity = this->mVelocity;

	pos = previousPos + previousVelocity * this->mDeltaTime;

	if (addGravity)
	{
		this->mVelocity = previousVelocity + this->mGravity * this->mDeltaTime;
		pos += this->mGravity * (this->mDeltaTime * this->mDeltaTime) / 2.0f;
	}
}

bool PhysicsEntity::Jump(Vec3 &pos)
{
	if (!this->mStates.isJumping)
	{
		this->mStates.isJumping = true;
		this->mVelocity.Y += JUMP_VELOCITY_DEFAULT;

		pos += this->mVelocity*this->mDeltaTime;

		return true;
	}
	else
	{
		return false;
	}
}



bool PhysicsEntity::FireProjectile(Vec3 &pos, Vec3 direction)
{
	if (!this->mStates.isActiveProjectile)
	{
		this->mStates.isActiveProjectile = true;
		this->mVelocity += direction;
		return true;
	}
	else
	{
		return false;
	}
}

bool PhysicsEntity::FireProjectileAt(Vec3 &pos, Vec3 target)
{
	if (!this->mStates.isActiveProjectile)
	{
		Vec3 delta, projectileVelocity, deltaXZPlane;
		float initVelocityX, initVelocityY, initVelocityZ,
			height, lenXZ, projectileConst;

		delta = target - pos;
		deltaXZPlane = delta;
		deltaXZPlane.Y = 0.0f;
		lenXZ = deltaXZPlane.Length();
		height = PROJECTILE_HEIGHT_DEFAULT;
		projectileConst = 0.95;

		//Use appropriate defined numbers depending on the length and height of the projectilemotion

		//Height
		if (delta.Y > 0.0f)
		{
			height = PROJECTILE_HEIGHT_HIGH;
		}
		else if (delta.Y < 0.0f)
		{
			height = PROJECTILE_HEIGHT_LOW;
		}

		//Length
		if (lenXZ < PROJECTILE_LENGTH_SHORT)
		{
			//Increase in order to prevent the throw from being too "weak"
			//and instead make the throw to go past the target in order to for it
			//to appear more like a throw
			projectileConst = 0.50;
		}
		else if (lenXZ > PROJECTILE_LENGTH_FAR)
		{
			//Reverse action here, when the target is very far away, increase
			//the constant  to make it not quite reach its target
			projectileConst = 1.10;
		}

		initVelocityX = delta.X / projectileConst;
		initVelocityZ = delta.Z / projectileConst;
		initVelocityY = sqrtf(0 - (2 * this->mGravity.Y * height));

		projectileVelocity = Vec3(initVelocityX, initVelocityY, initVelocityZ);

		this->mStates.isActiveProjectile = true;
		this->mVelocity += projectileVelocity;
		pos += this->mVelocity * this->mDeltaTime;

		return true;
	}
	else
	{
		return false;
	}
}

float PhysicsEntity::Lerp(float a, float b, float amount)
{
	return a + (b - a) * amount;
}

void PhysicsEntity::Walk(Vec3 &pos, float speed)
{
	this->mOrient.Walk(speed, pos);
}

void PhysicsEntity::Walk(Vec3 &pos)
{
	float speed = MOVEMENTSPEED_DEFAULT;
	this->mOrient.Walk(speed, pos);
}

void PhysicsEntity::Strafe(Vec3 &pos, float speed)
{
	this->mOrient.Strafe(speed, pos);
}
void PhysicsEntity::Strafe(Vec3 &pos)
{
	float speed = MOVEMENTSPEED_DEFAULT;
	this->mOrient.Strafe(speed, pos);
}

void PhysicsEntity::MoveRelativeVec3(Vec3 &pos, Vec3 &relativeVec, Vec3 &rot, float angleY)
{
	if (relativeVec != Vec3(0.0f, 0.0f, 0.0f))
	{
		Vec3 cRot, eLook, eRight, eUp, rVec, normal, cross;
		float s, c, angle;

		eLook = this->mOrient.GetLook();
		eRight = this->mOrient.GetRight();
		eUp = this->mOrient.GetUp();
		rVec = relativeVec;

		rVec.Y = 0;
		rVec.Normalize();
		s = eLook.Cross(rVec).Length();
		c = eLook.Dot(rVec);

		angle = atan2f(s, c);

		normal = eLook.Cross(rVec);
		cross = normal.Cross(rVec);
		if (normal.Y < 0.0f)
		{
			angle = -angle;
		}

		if (this->mOrient.GetRotRelativeCam().Y != ToDegrees(angleY))
		{
			this->mOrient.SetRotRelativeCam(Vec3(0.0f, ToRadians(angleY), 0.0f));
			angle += ToRadians(angleY);
		}

		this->RotateY(rot, ToDegrees(angle));
	}
	float speed = MOVEMENTSPEED_DEFAULT;
	this->Walk(pos);
}

void PhysicsEntity::MoveRelativeVec3(Vec3 &pos, Vec3 &relativeVec, float speed, Vec3 &rot, float angleY)
{
	if (relativeVec != Vec3(0.0f, 0.0f, 0.0f))
	{
		Vec3 cRot, eLook, eRight, eUp, rVec, normal, cross;
		float s, c, angle;

		eLook = this->mOrient.GetLook();
		eRight = this->mOrient.GetRight();
		eUp = this->mOrient.GetUp();
		rVec = relativeVec;

		rVec.Y = 0;
		rVec.Normalize();
		s = eLook.Cross(rVec).Length();
		c = eLook.Dot(rVec);

		angle = atan2f(s, c);

		normal = eLook.Cross(rVec);
		cross = normal.Cross(rVec);
		if (normal.Y < 0.0f)
		{
			angle = -angle;
		}

		if (this->mOrient.GetRotRelativeCam().Y != ToDegrees(angleY))
		{
			this->mOrient.SetRotRelativeCam(Vec3(0.0f, ToRadians(angleY), 0.0f));
			angle += ToRadians(angleY);
		}

		this->RotateY(rot, ToDegrees(angle));
	} 
	this->Walk(pos, speed);
}

//moving the entity that is being pushed by another entity
Vec3 PhysicsEntity::MovePushed(Vec3 pos)
{
	pos += this->mPushDirection * (this->mDeltaTime * 5);
	return pos;
}

void PhysicsEntity::RotateX(Vec3 &rot, float angleX)
{
	angleX = ToRadians(angleX);
	this->mOrient.RotateX(rot, angleX);
}

void PhysicsEntity::RotateY(Vec3 &rot, float angleY)
{
	angleY = ToRadians(angleY);
	this->mOrient.RotateY(rot, angleY);
}

void PhysicsEntity::RotateZ(Vec3 &rot, float angleZ)
{
	angleZ = ToRadians(angleZ);
	this->mOrient.RotateZ(rot, angleZ);
}

void PhysicsEntity::ResetRot(Vec3 &rot)
{
	this->mOrient.ResetRotationXYZ(rot, X);
	this->mOrient.ResetRotationXYZ(rot, Y);
	this->mOrient.ResetRotationXYZ(rot, Z);
}

void PhysicsEntity::SetOrientation(Vec3 look, Vec3 right, Vec3 up)
{
	this->mOrient.SetOrientation(look, right, up);
}

void PhysicsEntity::SetPushDirection(Vec3 direction)
{
	this->mPushDirection = direction;
}

void PhysicsEntity::MoveUp(Vec3 &pos)
{
}

void PhysicsEntity::MoveDown(Vec3 &pos)
{
}
