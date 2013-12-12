#ifndef PHYSICS_H
#define PHYSICS_H


#include "shared/platform.h"
#include <DirectXMath.h>
#include <ctime>
#include "shared/Vec3.h"
//#include "Vec3.h"
#include <math.h>
#include "Orientation.h"

const float PI = 3.141592653589f;

#define DEFAULT_GRAVITY Vec3(0.0f, -9.82f, 0.0f);
#define DEFAULT_MASS 50.0f; //In KG
#define DEFAULT_VELOCITY Vec3(0.0f, 0.0f, 0.0f);
#define DEFAULT_MOVEMENTSPEED 10.0f;
#define DEFAULT_JUMP_VELOCITY 25.0f;


class DLL_API Physics
{
private:
	Vec3 gravity;
	Vec3 velocity;
	float mass;
	float projectileAngle;
	float dt;
	bool jumping;
	Orientation orient;
public:
	Physics();
	Physics(Vec3 pos);
	Physics(const Physics& other);
	virtual ~Physics();


	void update(float dt); //Call each frame (so far only an update of delta time)
	bool jump(Vec3 &pos); //perform a jump on the given floats that represents a position
	void addGravityCalc(Vec3 &pos, Vec3 &velocity); //apply gravity the given vector that represents a pos
	void addGravityCalc(Vec3 &pos); // same story here, only position though
	void addProjectileCalc(Vec3 &pos, Vec3 &velocity, Vec3 &acceleration);
	void moveForward(Vec3 &pos, float speed);
	void moveBackward(Vec3 &pos, float speed);
	void moveRight(Vec3 &pos, float speed);
	void moveLeft(Vec3 &pos, float speed);
	void moveForward(Vec3 &pos);
	void moveBackward(Vec3 &pos);
	void moveRight(Vec3 &pos);
	void moveLeft(Vec3 &pos);
	void moveUp(Vec3 &pos);
	void moveDown(Vec3 &pos);
	void rotateX(Vec3 &rot, float angleX);
	void rotateY(Vec3 &rot, float angleY);
	void rotateZ(Vec3 &rot, float angleZ);
	void resetRot(Vec3 &rot);

	void setGravity(float x, float y, float z); //call to alter the effects of gravity, default gravity vector is (0, -9.82, 0) (gravity of earth).
	void setMass(float mass); //not relevant yet
	void setVelocity(Vec3 vel);
	void setJumping(bool value);

	float getMass() const;
	Vec3 getGravity() const;
	bool isJumping() const { return jumping; }

	static float toRadians(float degrees);
	static float toDegrees(float radians);
	static Vec3 toRadians(Vec3 degrees);
	static Vec3 toDegrees(Vec3 radians);

private:
	float lerp(float a, float b, float amount);
	void updateOrientPos(Vec3 pos);
};


#endif