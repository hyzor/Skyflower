#include "Physics.h"

Physics::Physics()
{
	this->start = std::clock();

	this->gravity = DEFAULT_GRAVITY;
	this->mass = DEFAULT_MASS;
	/*this->gDeltaTime = 0.0f;
	this->jDeltaTime = 0.0f;
	this->pDeltaTime = 0.0f;*/
	this->projectileAngle = 0.0f;
	this->dt = 0.0;
}


Physics::Physics(const Physics& other)
{
	this->start = std::clock();

	this->gravity = other.gravity;
	this->mass = other.mass;
	/*this->gDeltaTime = other.gDeltaTime;
	this->jDeltaTime = other.jDeltaTime;
	this->pDeltaTime = other.pDeltaTime;*/
	this->projectileAngle = other.projectileAngle;
	this->dt = 0.0;
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
}

void Physics::addGravityCalc(Vec3 &pos, Vec3 &velocity, Vec3 &acceleration)
{
	Vec3 previousVelocity = velocity;
	Vec3 previousPos = pos;
	Vec3 previousAcc = acceleration;

	velocity = previousVelocity + this->gravity * this->dt;
	pos = previousPos + previousVelocity * this->dt + this->gravity * (this->dt * this->dt) / 2.0f;
}

void Physics::addGravityCalc(Vec3 &pos, Vec3 &velocity)
{
	Vec3 previousVelocity = velocity;
	Vec3 previousPos = pos;

	velocity = previousVelocity + this->gravity * this->dt;
	pos = previousPos + previousVelocity * this->dt + this->gravity * (this->dt * this->dt) / 2.0f;
}

void Physics::addGravityCalc(Vec3 &pos)
{
	Vec3 previousPos = pos;
	Vec3 previousVelocity = DEFAULT_VELOCITY_FALL;

	previousVelocity = previousVelocity + this->gravity * this->dt;
	pos = previousPos + previousVelocity * this->dt + this->gravity * (this->dt * this->dt) / 2.0f;
}

void Physics::moveForward(Vec3 &pos, Vec3 look, float speed)
{
	Vec3 velocity = Vec3(1.0f, 1.0f, 1.0f);
	velocity = look * speed * this->dt;
	pos += velocity;
}

void Physics::moveBackward(Vec3 &pos, Vec3 look, float speed)
{
	Vec3 velocity = Vec3(1.0f, 1.0f, 1.0f);
	velocity = look * speed * this->dt;
	pos -= velocity;
}

void Physics::moveRight(Vec3 &pos, Vec3 right, float speed)
{
	Vec3 velocity = Vec3(1.0f, 1.0f, 1.0f);
	velocity = right * speed * this->dt;
	pos += velocity;
}

void Physics::moveLeft(Vec3 &pos, Vec3 right, float speed)
{
	Vec3 velocity = Vec3(1.0f, 1.0f, 1.0f);
	velocity = right * speed * this->dt;
	pos -= velocity;
}

void Physics::moveForward(Vec3 &pos, Vec3 look)
{
	float speed = DEFAULT_MOVEMENTSPEED;
	Vec3 velocity = Vec3(1.0f, 1.0f, 1.0f);

	velocity = look * speed * this->dt;
	pos += velocity;
}
void Physics::moveBackward(Vec3 &pos, Vec3 look)
{
	float speed = DEFAULT_MOVEMENTSPEED;
	Vec3 velocity = Vec3(1.0f, 1.0f, 1.0f);

	velocity = look * speed * this->dt;
	pos -= velocity;
}
void Physics::moveRight(Vec3 &pos, Vec3 right)
{
	float speed = DEFAULT_MOVEMENTSPEED;
	Vec3 velocity = Vec3(1.0f, 1.0f, 1.0f);

	velocity = right * speed * this->dt;
	pos += right;
}
void Physics::moveLeft(Vec3 &pos, Vec3 right)
{
	float speed = DEFAULT_MOVEMENTSPEED;
	Vec3 velocity = Vec3(1.0f, 1.0f, 1.0f);

	velocity = right * speed * this->dt;
	pos -= right;
}

