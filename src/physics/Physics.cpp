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
	this->totalRot = 0.0f;
	this->velocity = Vec3::Zero();
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
	this->totalRot = 0.0f;
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

//void Physics::moveRight(Vec3 &pos, Vec3 right, Vec3 &rotation, Vec3 cameraLookAt, float speed)
//{
//	Vec3 velocity = Vec3(1.0f, 1.0f, 1.0f);
//	if (totalRot < 90)
//	{
//		totalRot += 0.1 * dt;
//	}
//	float rad = totalRot * (3.14 / 180);
//	float x, z, dx, dz, cs, sn;
//	cs = cosf(rad);
//	sn = sinf(rad);
//
//	x = cameraLookAt.X;
//	z = cameraLookAt.Z;
//
//	dx = x * cs - z * sn;
//	dz = x * sn + z * cs;
//
//	velocity.X = dx;
//	velocity.Z = dz;
//
//	pos += velocity;
//}

void Physics::rotateY()
{

}
void Physics::move(Vec3 &pos, Vec3 cameraLookAt, Vec3 dir, float speed)
{
	float target = (Vec3(0.0f, 0.0f, 1.0f).Dot(dir));

	Vec3 velocity;
	if (totalRot < target)
	{
		totalRot += 0.1 * dt;
	}
	float rad = totalRot * (PI / 180);
	float x, z, dx, dz, cs, sn;
	cs = cosf(rad);
	sn = sinf(rad);

	x = cameraLookAt.X;
	z = cameraLookAt.Z;

	dx = x * cs - z * sn;
	dz = x * sn + z * cs;

	velocity.X = dx;
	velocity.Z = dz;

	pos += velocity * this->dt;
}
void Physics::moveLeft(Vec3 &pos, Vec3 cameraLookAt, float speed)
{
	Vec3 velocity;
	if (totalRot < 90)
	{
		totalRot += 0.1 * dt;
	}
	float rad = totalRot * (PI / 180);
	float x, z, dx, dz, cs, sn;
	cs = cosf(rad);
	sn = sinf(rad);

	x = cameraLookAt.X;
	z = cameraLookAt.Z;

	dx = x * cs - z * sn;
	dz = x * sn + z * cs;

	velocity.X = dx;
	velocity.Z = dz;

	pos += velocity;
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

void Physics::jump(Vec3 &pos)
{
	if (!isJumping)
	{
		jumping = true;
		velocity.Y += DEFAULT_JUMP_VELOCITY;
	}
}

float Physics::lerp(float a, float b, float amount)
{
	return a + (b - a) * amount;
}