#ifndef PHYSICS_H
#define PHYSICS_H


#include "shared/platform.h"
#include <DirectXMath.h>
#include <ctime>
#include "shared/Vec3.h"
#include <math.h>
#include "Orientation.h"
#include <string>
#include <iostream>
using namespace std;

const float PI = 3.141592653589f;

#define DEFAULT_GRAVITY Vec3(0.0f, -120.82f, 0.0f)
#define DEFAULT_MASS 50.0f //In KG
#define DEFAULT_VELOCITY Vec3(0.0f, 0.0f, 0.0f)
#define DEFAULT_MOVEMENTSPEED 50.0f
#define DEFAULT_JUMP_VELOCITY 35.0f

//Currently sort of a helper class, to be held by each entity to make sure that the velocity and other entity-specific data are kept by and manipulated only by the right entity
class DLL_API Physics
{
private:
	//used for jumping and gravity calculation
	Vec3 gravity;
	Vec3 velocity;
	Vec3 pushDirection;

	//not relevant yet
	float mass; 
	float projectileAngle;
	
	float dt;

	//used to keep track of an entitys state regarding jumping, in order tp prevent/enable it dynamically
	bool jumping;
	bool isMoving;

	//A simple class that keeps track of and manipulates the orientation (meaning rotation, look/right and up -vectors) of the entity holding the physics instance
	Orientation orient;
public:
	//basic constructors
	Physics();
	Physics(Vec3 pos);
	Physics(const Physics& other);
	virtual ~Physics();

	//Call each frame, updates delta and orientation
	void update(float dt);

	//perform a jump on the given vector that represents a position by increasing velocity in Y-axis
	bool jump(Vec3 &pos); 

	// Dessa gör mer än att bara applicera gravitationen, utan att kalla på någon av dessa
	// funktioner fungerar ingen velocity alls, alltså kan man inte hoppa om man inte kallar
	// någon av dessa funktion. Därav den dumma addGravity boolen.
	//apply gravity the given vector that represents a position
	void addGravityCalc(Vec3 &pos, Vec3 &velocity, bool addGravity); 
	void addGravityCalc(Vec3 &pos, bool addGravity);

	//to be used for projectile calculations
	void addProjectileCalc(Vec3 &pos, Vec3 &velocity, Vec3 &acceleration);

	//walk along the look vector kept in Orientation
	void walk(Vec3 &pos, float speed);
	void walk(Vec3 &pos);

	//walk along the right vector kept in Orientation
	void strafe(Vec3 &pos, float speed);
	void strafe(Vec3 &pos);

	void moveUp(Vec3 &pos);
	void moveDown(Vec3 &pos);

	Vec3 movePushed(Vec3 pos);

	//rotate in relation to given vector plus an offset (angle)
	void moveRelativeVec3(Vec3 &pos, Vec3 &relativeVec, Vec3 &rot, float angleY);
	void moveRelativeVec3(Vec3 &pos, Vec3 &relativeVec, float speed,Vec3 &rot, float angleY);
	
	//rotate the different vectors in Orientation
	void rotateX(Vec3 &rot, float angleX);
	void rotateY(Vec3 &rot, float angleY);
	void rotateZ(Vec3 &rot, float angleZ);
	void resetRot(Vec3 &rot);

	//set parameters of calculations, achieveing different effects. And other setfunctions
	void setGravity(float x, float y, float z); 
	void setMass(float mass); 
	void setVelocity(Vec3 vel);
	void setJumping(bool value);
	void setOrientation(Vec3 look, Vec3 right, Vec3 up);
	void setIsMoving(bool state);
	void setPushDirection(Vec3 direction);

	//Standard getfunctions.
	float getMass() const;
	Vec3 getGravity() const;
	bool isJumping() const { return jumping; }
	Vec3 getLook() const; //Fetched from Orientation
	Vec3 getRight() const; //Fetched from Orientation
	Vec3 getUp() const; //Fetched from Orientation
	Vec3 getVelocity() const; //Fetched from EntityManager (used for checking if you can push something)
	bool getIsMoving() const { return isMoving; }

	static float toRadians(float degrees);
	static float toDegrees(float radians);
	static Vec3 toRadians(Vec3 degrees);
	static Vec3 toDegrees(Vec3 radians);

private:
	float lerp(float a, float b, float amount);
};


#endif