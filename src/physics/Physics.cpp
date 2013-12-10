#include "Physics.h"

Physics::Physics()
{
	this->gravity = DEFAULT_GRAVITY;
	this->mass = DEFAULT_MASS;
	this->projectileAngle = 0.0f;
	this->dt = 0.0;
	this->velocity = DEFAULT_VELOCITY;
}

Physics::Physics(Vec3 pos)
{
	this->gravity = DEFAULT_GRAVITY;
	this->mass = DEFAULT_MASS;
	this->projectileAngle = 0.0f;
	this->dt = 0.0;
	this->velocity = DEFAULT_VELOCITY;
	this->updateOrientPos(pos);
}


Physics::Physics(const Physics& other)
{
	this->gravity = other.gravity;
	this->mass = other.mass;
	this->projectileAngle = other.projectileAngle;
	this->dt = 0.0;
	this->velocity = other.velocity;
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
	this->orient.setPos(pos);
}

void Physics::addGravityCalc(Vec3 &pos)
{
	Vec3 previousPos = pos;
	Vec3 previousVelocity = this->velocity;

	this->velocity = previousVelocity + this->gravity * this->dt;
	pos = previousPos + previousVelocity * this->dt + this->gravity * (this->dt * this->dt) / 2.0f;
	this->orient.setPos(pos);
}

void Physics::jump(Vec3 &pos)
{
	if (!isJumping())
	{
		jumping = true;
		velocity.Y += DEFAULT_JUMP_VELOCITY;
	}
}



float Physics::lerp(float a, float b, float amount)
{
	return a + (b - a) * amount;
}

void Physics::moveForward(Vec3 &pos, float speed)
{
	if (this->orient.getRotY() != toRadians(0.0f))
	{
		this->orient.resetRotationXYZ(Axis::Y);
	}
	this->orient.setPos(pos);
	this->orient.walk(speed, pos);
}

void Physics::moveBackward(Vec3 &pos, float speed)
{
	if (this->orient.getRotY() != toRadians(180.0f) && this->orient.getRotY() != toRadians(0.0f))
	{
		this->orient.resetRotationXYZ(Axis::Y);
	}
	if (this->orient.getRotY() == toRadians(0.0f))
	{
		this->orient.rotateY(toRadians(180.0f));
	}
	this->orient.setPos(pos);
	this->orient.walk(speed, pos);
}

void Physics::moveRight(Vec3 &pos, float speed)
{
	if (this->orient.getRotY() != toRadians(90.0f) && this->orient.getRotY() != toRadians(0.0f))
	{
		this->orient.resetRotationXYZ(Axis::Y);
	}
	if (this->orient.getRotY() == toRadians(0.0f))
	{
		this->orient.rotateY(toRadians(90.0f));
	}
	this->orient.setPos(pos);
	this->orient.walk(speed, pos);
}

void Physics::moveLeft(Vec3 &pos, float speed)
{
	if (this->orient.getRotY() != toRadians(-90.0f) && this->orient.getRotY() != toRadians(0.0f))
	{
		this->orient.resetRotationXYZ(Axis::Y);
	}
	if (this->orient.getRotY() == toRadians(0.0f))
	{
		this->orient.rotateY(toRadians(-90.0f));
	}
	this->orient.setPos(pos);
	this->orient.walk(speed, pos);
}

void Physics::moveForward(Vec3 &pos)
{
	float speed = DEFAULT_MOVEMENTSPEED;
	if (this->orient.getRotY() != toRadians(0.0f))
	{
		this->orient.resetRotationXYZ(Axis::Y);
	}
	this->orient.setPos(pos);
	this->orient.walk(speed, pos);
}

void Physics::moveBackward(Vec3 &pos)
{
	float speed = DEFAULT_MOVEMENTSPEED;
	if (this->orient.getRotY() != toRadians(180.0f) && this->orient.getRotY() != toRadians(0.0f))
	{
		this->orient.resetRotationXYZ(Axis::Y);
	}
	if (this->orient.getRotY() == toRadians(0.0f))
	{
		this->orient.rotateY(toRadians(180.0f));
	}
	this->orient.setPos(pos);
	this->orient.walk(speed, pos);
}

void Physics::moveRight(Vec3 &pos)
{
	float speed = DEFAULT_MOVEMENTSPEED;
	if (this->orient.getRotY() != toRadians(90.0f) && this->orient.getRotY() != toRadians(0.0f))
	{
		this->orient.resetRotationXYZ(Axis::Y);
	}
	if (this->orient.getRotY() == toRadians(0.0f))
	{
		this->orient.rotateY(toRadians(90.0f));
	}
	this->orient.setPos(pos);
	this->orient.walk(speed, pos);
}

void Physics::moveLeft(Vec3 &pos)
{
	float speed = DEFAULT_MOVEMENTSPEED;
	if (this->orient.getRotY() != toRadians(-90.0f) && this->orient.getRotY() != toRadians(0.0f))
	{
		this->orient.resetRotationXYZ(Axis::Y);
	}
	if (this->orient.getRotY() == toRadians(0.0f))
	{
		this->orient.rotateY(toRadians(-90.0f));
	}
	this->orient.setPos(pos);
	this->orient.walk(speed, pos);
}

void Physics::moveUp(Vec3 &pos)
{
//Do something
}

void Physics::moveDown(Vec3 &pos)
{
//Do something
}



void Physics::updateOrientPos(Vec3 pos)
{
	this->orient.setPos(pos);
}

float Physics::toRadians(float degrees)
{
	return (degrees * PI / 180.0f);
}