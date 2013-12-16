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

Vec3 Physics::getLook() const
{
	return this->orient.getLook();
}

Vec3 Physics::getRight() const
{
	return this->orient.getRight();
}

Vec3 Physics::getUp() const
{
	return this->orient.getUp();
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

bool Physics::jump(Vec3 &pos)
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

void Physics::setJumping(bool jump)
{
	jumping = jump;
}


float Physics::lerp(float a, float b, float amount)
{
	return a + (b - a) * amount;
}

void Physics::walk(Vec3 &pos, float speed)
{
	this->orient.walk(speed, pos);
}

void Physics::walk(Vec3 &pos)
{
	float speed = DEFAULT_MOVEMENTSPEED;
	this->orient.walk(speed, pos);
}

void Physics::strafe(Vec3 &pos, float speed)
{
	this->orient.strafe(speed, pos);
}
void Physics::strafe(Vec3 &pos)
{
	float speed = DEFAULT_MOVEMENTSPEED;
	this->orient.strafe(speed, pos);
}

void Physics::moveRelativeVec3(Vec3 &pos, Vec3 &relativeVec, Vec3 &rot, float angleY)
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

void Physics::moveRelativeVec3(Vec3 &pos, Vec3 &relativeVec, float speed, Vec3 &rot, float angleY)
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

void Physics::setOrientation(Vec3 look, Vec3 right, Vec3 up)
{
	this->orient.setOrientation(look, right, up);
}

void Physics::moveUp(Vec3 &pos)
{
}

void Physics::moveDown(Vec3 &pos)
{
}
