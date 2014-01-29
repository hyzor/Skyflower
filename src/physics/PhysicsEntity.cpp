#include "PhysicsEntity.h"

// Must be included last!
#include "shared/debug.h"

Vec3 PhysicsEntity::mGlobalGravity = Vec3(0.0f, 0.0f, 0.0f);

PhysicsEntity::PhysicsEntity()
{
	this->gravity = DEFAULT_GRAVITY;
	this->mass = DEFAULT_MASS;
	this->projectileAngle = 0.0f;
	this->dt = 0.0;
	this->velocity = DEFAULT_VELOCITY;
	this->jumping = false;
	this->isMoving = false;
	this->isBeingPushed = false;
}

PhysicsEntity::PhysicsEntity(Vec3 pos)
{
	this->gravity = DEFAULT_GRAVITY;
	this->mass = DEFAULT_MASS;
	this->projectileAngle = 0.0f;
	this->dt = 0.0;
	this->velocity = DEFAULT_VELOCITY;
	this->jumping = false;
	this->isMoving = false;
	this->isBeingPushed = false;
}


PhysicsEntity::PhysicsEntity(const PhysicsEntity& other)
{
	this->gravity = other.gravity;
	this->mass = other.mass;
	this->projectileAngle = other.projectileAngle;
	this->dt = 0.0;
	this->velocity = other.velocity;
	this->jumping = other.jumping;
}


PhysicsEntity::~PhysicsEntity(){}

void PhysicsEntity::setGravity(float x, float y, float z)
{
	this->gravity.X = x;
	this->gravity.Y = y;
	this->gravity.Z = z;
}

void PhysicsEntity::SetGlobalGravity(Vec3 gravity)
{
	PhysicsEntity::mGlobalGravity = gravity;
}

void PhysicsEntity::setMass(float m)
{
	this->mass = m;
}

float PhysicsEntity::getMass() const
{
	return this->mass;
}

Vec3 PhysicsEntity::getGravity() const
{
	return this->gravity;
}

Vec3 PhysicsEntity::getLook() const
{
	return this->orient.getLook();
}

Vec3 PhysicsEntity::getRight() const
{
	return this->orient.getRight();
}

Vec3 PhysicsEntity::getUp() const
{
	return this->orient.getUp();
}

Vec3 PhysicsEntity::getVelocity() const
{
	return this->velocity;
}
void PhysicsEntity::update(float dt)
{
	this->dt = dt;
	this->orient.update(dt);
}

void PhysicsEntity::addGravityCalc(Vec3 &pos, Vec3 &velocity, bool addGravity)
{
	Vec3 previousVelocity = velocity;
	Vec3 previousPos = pos;

	velocity = previousVelocity;
	pos = previousPos + previousVelocity * this->dt;
	//this->orient.setPos(pos);

	if (addGravity)
	{
		velocity += this->gravity * this->dt;
		pos += this->gravity * (this->dt * this->dt) / 2.0f;
	}
}

void PhysicsEntity::setVelocity(Vec3 vel)
{
	this->velocity = vel;
}

void PhysicsEntity::addGravityCalc(Vec3 &pos, bool addGravity)
{
	Vec3 previousPos = pos;
	Vec3 previousVelocity = this->velocity;

	pos = previousPos + previousVelocity * this->dt;

	if (addGravity)
	{
		this->velocity = previousVelocity + this->gravity * this->dt;
		pos += this->gravity * (this->dt * this->dt) / 2.0f;
	}
}

bool PhysicsEntity::jump(Vec3 &pos)
{
	if (!isJumping())
	{
		this->jumping = true;
		velocity.Y += DEFAULT_JUMP_VELOCITY;

		pos += velocity*dt;

		return true;
	}
	else
	{
		return false;
	}
}

void PhysicsEntity::setJumping(bool jump)
{
	jumping = jump;
}

void PhysicsEntity::setIsMoving(bool state)
{
	this->isMoving = state;
}

void PhysicsEntity::setIsBeingPushed(bool state)
{
	this->isBeingPushed = state;
}

float PhysicsEntity::lerp(float a, float b, float amount)
{
	return a + (b - a) * amount;
}

void PhysicsEntity::walk(Vec3 &pos, float speed)
{
	this->orient.walk(speed, pos);
}

void PhysicsEntity::walk(Vec3 &pos)
{
	float speed = DEFAULT_MOVEMENTSPEED;
	this->orient.walk(speed, pos);
}

void PhysicsEntity::strafe(Vec3 &pos, float speed)
{
	this->orient.strafe(speed, pos);
}
void PhysicsEntity::strafe(Vec3 &pos)
{
	float speed = DEFAULT_MOVEMENTSPEED;
	this->orient.strafe(speed, pos);
}

void PhysicsEntity::moveRelativeVec3(Vec3 &pos, Vec3 &relativeVec, Vec3 &rot, float angleY)
{
	if (relativeVec != Vec3(0.0f, 0.0f, 0.0f))
	{
		Vec3 cRot, eLook, eRight, eUp, rVec, normal, cross;
		float s, c, angle;

		eLook = this->orient.getLook();
		eRight = this->orient.getRight();
		eUp = this->orient.getUp();
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

		if (this->orient.getRotRelativeCam().Y != toDegrees(angleY))
		{
			this->orient.setRotRelativeCam(Vec3(0.0f, toRadians(angleY), 0.0f));
			angle += toRadians(angleY);
		}

		this->rotateY(rot, toDegrees(angle));
	}
	float speed = DEFAULT_MOVEMENTSPEED;
	this->walk(pos);
}

void PhysicsEntity::moveRelativeVec3(Vec3 &pos, Vec3 &relativeVec, float speed, Vec3 &rot, float angleY)
{
	if (relativeVec != Vec3(0.0f, 0.0f, 0.0f))
	{
		Vec3 cRot, eLook, eRight, eUp, rVec, normal, cross;
		float s, c, angle;

		eLook = this->orient.getLook();
		eRight = this->orient.getRight();
		eUp = this->orient.getUp();
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

		if (this->orient.getRotRelativeCam().Y != toDegrees(angleY))
		{
			this->orient.setRotRelativeCam(Vec3(0.0f, toRadians(angleY), 0.0f));
			angle += toRadians(angleY);
		}

		this->rotateY(rot, toDegrees(angle));
	} 
	this->walk(pos, speed);
}

//moving the entity that is peing pushed by another entity
Vec3 PhysicsEntity::movePushed(Vec3 pos)
{
	pos += pushDirection * (dt * 5);
	return pos;
}


float PhysicsEntity::toRadians(float degrees)
{
	return (degrees * PI / 180.0f);
}

float PhysicsEntity::toDegrees(float radians)
{
	return (radians * 180.0f / PI);
}

Vec3 PhysicsEntity::toRadians(Vec3 degrees)
{
	return Vec3(toRadians(degrees.X), toRadians(degrees.Y), toRadians(degrees.Z));
}

Vec3 PhysicsEntity::toDegrees(Vec3 radians)
{
	return Vec3(toDegrees(radians.X), toDegrees(radians.Y), toDegrees(radians.Z));
}

void PhysicsEntity::rotateX(Vec3 &rot, float angleX)
{
	angleX = toRadians(angleX);
	this->orient.rotateX(rot, angleX);
}

void PhysicsEntity::rotateY(Vec3 &rot, float angleY)
{
	angleY = toRadians(angleY);
	this->orient.rotateY(rot, angleY);
}

void PhysicsEntity::rotateZ(Vec3 &rot, float angleZ)
{
	angleZ = toRadians(angleZ);
	this->orient.rotateZ(rot, angleZ);
}

void PhysicsEntity::resetRot(Vec3 &rot)
{
	this->orient.resetRotationXYZ(rot, X);
	this->orient.resetRotationXYZ(rot, Y);
	this->orient.resetRotationXYZ(rot, Z);
}

void PhysicsEntity::setOrientation(Vec3 look, Vec3 right, Vec3 up)
{
	this->orient.setOrientation(look, right, up);
}

void PhysicsEntity::setPushDirection(Vec3 direction)
{
	this->pushDirection = direction;
}

void PhysicsEntity::moveUp(Vec3 &pos)
{
}

void PhysicsEntity::moveDown(Vec3 &pos)
{
}
