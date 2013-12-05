#ifndef PHYSICS_H
#define PHYSICS_H


#include "shared/platform.h"
#include <DirectXMath.h>
#include <ctime>
#include "Vec3.h"
#include <math.h>

#define DEFAULT_GRAVITY Vec3(0.0f, -9.82f, 0.0f);
#define DEFAULT_MASS 50.0f; //In KG
#define DEFAULT_VELOCITY_FALL Vec3(0.0f, -1.0f, 0.0f);
#define DEFAULT_MOVEMENTSPEED 1.0f;
#define DEFAULT_JUMP_VELOCITY 10.0f

const float PI = 3.141592653589;
class DLL_API Physics
{
private:
	std::clock_t start;
	Vec3 gravity;
	Vec3 velocity;
	float mass;
	float projectileAngle;
	float dt;
	float totalRot;
	bool jumping;

	//float gDeltaTime; //used for gravity related calculations
	//float jDeltaTime; //used for jump related calculations
	//float pDeltaTime; //used for projectile related calculations



public:
	Physics();
	Physics(const Physics& other);
	virtual ~Physics();


	void update(float dt); //Call each frame (so far only an update of delta time)
	void jump(Vec3 &pos); //perform a jump on the given floats that represents a position
	void addGravityCalc(Vec3 &pos, Vec3 &velocity, Vec3 &acceleration); //apply gravity to the given input representing a position, should be called every frame (unless collisioncheck confirms the object is not airborne)
	void addGravityCalc(Vec3 &pos, Vec3 &velocity); //same operation as function above but with no need for acceleration
	void addGravityCalc(Vec3 &pos); // same story here, only position though
	void addProjectileCalc(Vec3 &pos, Vec3 &velocity, Vec3 &acceleration);
	
	void moveForward(Vec3 &pos, Vec3 look, float speed);
	void moveBackward(Vec3 &pos, Vec3 look, float speed);
	//void moveRight(Vec3 &pos, Vec3 right, Vec3 look, Vec3 cameraLookAt, float speed);
	void moveLeft(Vec3 &pos,Vec3 cameraLookAt, float speed);

	void move(Vec3 &pos, Vec3 cameraLookAt, Vec3 dir, float speed);

	void rotateY(); //rotate the look vector around its position

	void moveForward(Vec3 &pos, Vec3 look);
	void moveBackward(Vec3 &pos, Vec3 look);
	void moveRight(Vec3 &pos, Vec3 right);
	void moveLeft(Vec3 &pos, Vec3 right);

	void setGravity(float x, float y, float z); //call to alter the effects of gravity, default gravity vector is (0, -9.82, 0) (gravity of earth).
	void setMass(float mass); //not relevant yet

	float getMass() const;
	Vec3 getGravity() const;
	bool isJumping() const { return jumping; }

private:
	float lerp(float a, float b, float amount);
};


#endif