#ifndef PHYSICS_H
#define PHYSICS_H


#include "shared/platform.h"
#include <DirectXMath.h>
#include <ctime>
#include "shared/Vec3.h"

#define DEFAULT_GRAVITY Vec3(0.0f, -9.82f, 0.0f);
#define DEFAULT_MASS 50.0f; //In KG
#define DEFAULT_VELOCITY_FALL Vec3(0.0f, -1.0f, 0.0f);
#define DEFAULT_MOVEMENTSPEED 1.0f;

class DLL_API Physics
{
private:
	std::clock_t start;
	Vec3 gravity;
	float mass;
	float projectileAngle;
	float dt;

	//float gDeltaTime; //used for gravity related calculations
	//float jDeltaTime; //used for jump related calculations
	//float pDeltaTime; //used for projectile related calculations



public:
	Physics();
	Physics(const Physics& other);
	virtual ~Physics();


	void update(float dt); //Call each frame (so far only an update of delta time)
	void jumpCalc(Vec3 &pos, Vec3 &velocity, Vec3 &acceleration); //perform a jump on the given floats that represents a position
	void addGravityCalc(Vec3 &pos, Vec3 &velocity, Vec3 &acceleration); //apply gravity to the given input representing a position, should be called every frame (unless collisioncheck confirms the object is not airborne)
	void addGravityCalc(Vec3 &pos, Vec3 &velocity); //same operation as function above but with no need for acceleration
	void addGravityCalc(Vec3 &pos); // same story here, only position though
	void addProjectileCalc(Vec3 &pos, Vec3 &velocity, Vec3 &acceleration);
	
	void moveForward(Vec3 &pos, Vec3 look, float speed);
	void moveBackward(Vec3 &pos, Vec3 look, float speed);
	void moveRight(Vec3 &pos, Vec3 right, float speed);
	void moveLeft(Vec3 &pos, Vec3 right, float speed);
	void moveForward(Vec3 &pos, Vec3 look);
	void moveBackward(Vec3 &pos, Vec3 look);
	void moveRight(Vec3 &pos, Vec3 right);
	void moveLeft(Vec3 &pos, Vec3 right);

	void setGravity(float x, float y, float z); //call to alter the effects of gravity, default gravity vector is (0, -9.82, 0) (gravity of earth).
	void setMass(float mass); //not relevant yet

	float getMass() const;
	Vec3 getGravity() const;

};


#endif