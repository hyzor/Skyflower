#ifndef PHYSICS_PHYSICSENTITYIMPL_H
#define PHYSICS_PHYSICSENTITYIMPL_H

#include <DirectXMath.h>
#include <math.h>

#include "shared/platform.h"
#include "shared/Vec3.h"
#include "PhysicsHelper.h"
#include "PhysicsEntity.h"

using namespace std;
using namespace PhysicsHelper;

//Currently sort of a helper class, to be held by each entity to make sure that the velocity and other entity-specific data are kept by and manipulated only by the right entity
class PhysicsEntityImpl : public PhysicsEntity
{
private:
	//used for jumping and gravity calculation
	Vec3 mGravity;
	Vec3 mVelocity;
	Vec3 mPushDirection;

	//not relevant yet
	float mMass; 
	float mProjectileAngle;
	float mDeltaTime;

	//A simple class that keeps track of and manipulates the orientation (meaning rotation, look/right and up -vectors) of the entity holding the physics instance
	Orientation mOrient;

	//A struct containing bools to keep track of various states
	PhysicsEntityStates mStates;

	//A "common" gravity that is applied for all the entities
	static Vec3 mGlobalGravity;
public:
	//basic constructors
	PhysicsEntityImpl();
	PhysicsEntityImpl(Vec3 pos);
	PhysicsEntityImpl(const PhysicsEntityImpl& other);
	virtual ~PhysicsEntityImpl();

	//Call each frame, updates delta and orientation
	void Update(float dt);

	//perform a jump on the given vector that represents a position by increasing velocity in Y-axis
	bool Jump(Vec3 &pos); 

	// Dessa gör mer än att bara applicera gravitationen, utan att kalla på någon av dessa
	// funktioner fungerar ingen velocity alls, alltså kan man inte hoppa om man inte kallar
	// någon av dessa funktion. Därav den dumma addGravity boolen.
	//apply gravity the given vector that represents a position
	void AddGravityCalc(Vec3 &pos, Vec3 &velocity, bool addGravity); 
	void AddGravityCalc(Vec3 &pos, bool addGravity);

	//to be used for projectile calculations
	bool FireProjectile(Vec3 &pos, Vec3 direction);
	bool FireProjectileAt(Vec3 &pos, Vec3 target);

	//walk along the look vector kept in Orientation
	void Walk(Vec3 &pos, float speed);
	void Walk(Vec3 &pos);

	//walk along the right vector kept in Orientation
	void Strafe(Vec3 &pos, float speed);
	void Strafe(Vec3 &pos);

	void MoveUp(Vec3 &pos);
	void MoveDown(Vec3 &pos);

	Vec3 MovePushed(Vec3 pos);

	//rotate in relation to given vector plus an offset (angle) and move
	void MoveRelativeVec3(Vec3 &pos, Vec3 &relativeVec, Vec3 &rot, float angleY);
	void MoveRelativeVec3(Vec3 &pos, Vec3 &relativeVec, float speed,Vec3 &rot, float angleY);
	
	//rotate the different vectors in Orientation
	void RotateX(Vec3 &rot, float angleX);
	void RotateY(Vec3 &rot, float angleY);
	void RotateZ(Vec3 &rot, float angleZ);
	void ResetRot(Vec3 &rot);

	//set parameters of calculations, achieveing different effects. And other setfunctions
	void SetGravity(Vec3 gravity); 
	void SetMass(float mass); 
	void SetVelocity(Vec3 vel);
	void SetOrientation(Vec3 look, Vec3 right, Vec3 up);
	void SetPushDirection(Vec3 direction);

	//Standard getfunctions.
	float GetMass() const;
	Vec3 GetGravity() const;
	PhysicsEntityStates* GetStates() { return &this->mStates; } //Not constant,
	Orientation GetOrientation() const { return this->mOrient; }
	Vec3 GetVelocity() const; //Fetched from EntityManager (used for checking if you can push something)

	//Set gravity that should affect all the entities
	static void SetGlobalGravity(Vec3 gravity);

private:
	float Lerp(float a, float b, float amount);
};


#endif