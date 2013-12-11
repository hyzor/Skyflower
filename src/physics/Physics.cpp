#include "Physics.h"

Physics::Physics()
{
	this->gravity = DEFAULT_GRAVITY;
	this->mass = DEFAULT_MASS;
	this->projectileAngle = 0.0f;
	this->dt = 0.0;
	this->velocity = DEFAULT_VELOCITY;
	this->jumping = false;
}

Physics::Physics(Vec3 pos)
{
	this->gravity = DEFAULT_GRAVITY;
	this->mass = DEFAULT_MASS;
	this->projectileAngle = 0.0f;
	this->dt = 0.0;
	this->velocity = DEFAULT_VELOCITY;
	this->jumping = false;
}


Physics::Physics(const Physics& other)
{
	this->gravity = other.gravity;
	this->mass = other.mass;
	this->projectileAngle = other.projectileAngle;
	this->dt = 0.0;
	this->velocity = other.velocity;
	this->jumping = other.jumping;
}


Physics::~Physics(){}

void Physics::setGravity(float x, float y, float z)
{
	this->gravity.X = x;
	this->gravity.Y = y;
	this->gravity.Z = z;
}

void Physics::setMass(float m)
{
	this->mass = m;
}

float Physics::getMass() const
{
	return this->mass;
}

Vec3 Physics::getGravity() const
{
	return this->gravity;
}

void Physics::update(float dt)
{
	this->dt = dt;
	this->orient.update(dt);
}

void Physics::addGravityCalc(Vec3 &pos, Vec3 &velocity)
{
	Vec3 previousVelocity = velocity;
	Vec3 previousPos = pos;

	velocity = previousVelocity + this->gravity * this->dt;
	pos = previousPos + previousVelocity * this->dt + this->gravity * (this->dt * this->dt) / 2.0f;
	//this->orient.setPos(pos);
}

void Physics::setVelocity(Vec3 vel)
{
	this->velocity = vel;
}

void Physics::addGravityCalc(Vec3 &pos)
{
	Vec3 previousPos = pos;
	Vec3 previousVelocity = this->velocity;

	this->velocity = previousVelocity + this->gravity * this->dt;
	pos = previousPos + previousVelocity * this->dt + this->gravity * (this->dt * this->dt) / 2.0f;
}

void Physics::jump(Vec3 &pos)
{
	if (!isJumping())
	{
		this->jumping = true;
		velocity.Y += DEFAULT_JUMP_VELOCITY;
	}
}

void Physics::setJumping(bool jump)
{
	jumping = jump;
}


float Physics::lerp(float a, float b, float amount)
{
	return a + (b - a) * amount;
}

void Physics::moveForward(Vec3 &pos, float speed)
{
	/*if (this->orient.getRotY() != toRadians(0.0f))
	{
		this->orient.resetRotationXYZ(Axis::Y);
	}
	this->orient.setPos(pos);*/
	this->orient.walk(speed, pos);
}

void Physics::moveBackward(Vec3 &pos, float speed)
{
	/*if (this->orient.getRotY() != toRadians(180.0f) && this->orient.getRotY() != toRadians(0.0f))
	{
		this->orient.resetRotationXYZ(Axis::Y);
	}
	if (this->orient.getRotY() == toRadians(0.0f))
	{
		this->orient.rotateY(toRadians(180.0f));
	}
	this->orient.setPos(pos);*/
	this->orient.walk(speed, pos);
}

void Physics::moveRight(Vec3 &pos, float speed)
{
	/*if (this->orient.getRotY() != toRadians(90.0f) && this->orient.getRotY() != toRadians(0.0f))
	{
		this->orient.resetRotationXYZ(Axis::Y);
	}
	if (this->orient.getRotY() == toRadians(0.0f))
	{
		this->orient.rotateY(toRadians(90.0f));
	}
	this->orient.setPos(pos);*/
	this->orient.walk(speed, pos);
}

void Physics::moveLeft(Vec3 &pos, float speed)
{
	/*if (this->orient.getRotY() != toRadians(-90.0f) && this->orient.getRotY() != toRadians(0.0f))
	{
		this->orient.resetRotationXYZ(Axis::Y);
	}
	if (this->orient.getRotY() == toRadians(0.0f))
	{
		this->orient.rotateY(toRadians(-90.0f));
	}
	this->orient.setPos(pos);*/
	this->orient.walk(speed, pos);
}

void Physics::moveForward(Vec3 &pos)
{
	float speed = DEFAULT_MOVEMENTSPEED;
	/*if (this->orient.getRotY() != toRadians(0.0f))
	{
		this->orient.resetRotationXYZ(Axis::Y);
	}
	this->orient.setPos(pos);*/
	this->orient.walk(speed, pos);
}

void Physics::moveBackward(Vec3 &pos)
{
	float speed = DEFAULT_MOVEMENTSPEED;
	/*if (this->orient.getRotY() != toRadians(180.0f) && this->orient.getRotY() != toRadians(0.0f))
	{
		this->orient.resetRotationXYZ(Axis::Y);
	}
	if (this->orient.getRotY() == toRadians(0.0f))
	{
		this->orient.rotateY(toRadians(180.0f));
	}
	this->orient.setPos(pos);*/
	this->orient.walk(speed, pos);
}

void Physics::moveRight(Vec3 &pos)
{
	float speed = DEFAULT_MOVEMENTSPEED;
	/*if (this->orient.getRotY() != toRadians(90.0f) && this->orient.getRotY() != toRadians(0.0f))
	{
		this->orient.resetRotationXYZ(Axis::Y);
	}
	if (this->orient.getRotY() == toRadians(0.0f))
	{
		this->orient.rotateY(toRadians(90.0f));
	}
	this->orient.setPos(pos);*/
	this->orient.walk(speed, pos);
}

void Physics::moveLeft(Vec3 &pos)
{
	float speed = DEFAULT_MOVEMENTSPEED;
	/*if (this->orient.getRotY() != toRadians(-90.0f) && this->orient.getRotY() != toRadians(0.0f))
	{
		this->orient.resetRotationXYZ(Axis::Y);
	}
	if (this->orient.getRotY() == toRadians(0.0f))
	{
		this->orient.rotateY(toRadians(-90.0f));
	}
	this->orient.setPos(pos);*/
	this->orient.walk(speed, pos);
}

float Physics::toRadians(float degrees)
{
	return (degrees * PI / 180.0f);
}

float Physics::toDegrees(float radians)
{
	return (radians * 180.0f / PI);
}

Vec3 Physics::toRadians(Vec3 degrees)
{
	return Vec3(toRadians(degrees.X), toRadians(degrees.Y), toRadians(degrees.Z));
}

Vec3 Physics::toDegrees(Vec3 radians)
{
	return Vec3(toDegrees(radians.X), toDegrees(radians.Y), toDegrees(radians.Z));
}

void Physics::rotateX(Vec3 &rot, float angleX)
{
	angleX = toRadians(angleX);
	this->orient.rotateX(rot, angleX);
}

void Physics::rotateY(Vec3 &rot, float angleY)
{
	angleY = toRadians(angleY);
	this->orient.rotateY(rot, angleY);
}

void Physics::rotateZ(Vec3 &rot, float angleZ)
{
	angleZ = toRadians(angleZ);
	this->orient.rotateZ(rot, angleZ);
}

void Physics::resetRot(Vec3 &rot)
{
	this->orient.resetRotationXYZ(rot, X);
	this->orient.resetRotationXYZ(rot, Y);
	this->orient.resetRotationXYZ(rot, Z);
}

void Physics::moveUp(Vec3 &pos)
{
}

void Physics::moveDown(Vec3 &pos)
{
}
