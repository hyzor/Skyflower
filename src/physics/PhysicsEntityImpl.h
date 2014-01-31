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
	PhysicsEntityStates* mStates;

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
	bool Jump(Vec3 &pos, float forwardSpeed = MOVEMENTSPEED_DEFAULT);

	//Apply the velocity that has been accumulated during previous frames to the position
	void ApplyVelocityToPos(Vec3 &pos);

	// Dessa g�r mer �n att bara applicera gravitationen, utan att kalla p� n�gon av dessa
	// funktioner fungerar ingen velocity alls, allts� kan man inte hoppa om man inte kallar
	// n�gon av dessa funktion. D�rav den dumma addGravity boolen.
	//apply gravity the given vector that represents a position
	void AddGravityCalc(Vec3 &pos);

	//Fire a projectile in a given direction
	bool FireProjectile(Vec3 &pos, Vec3 direction);

	//Fire a projectile towards a specific target
	bool FireProjectileAt(Vec3 &pos, Vec3 target);

	//walk along the look vector kept in Orientation
	void Walk(Vec3 &pos, float speed = MOVEMENTSPEED_DEFAULT);

	//walk along the right vector kept in Orientation
	void Strafe(Vec3 &pos, float speed = MOVEMENTSPEED_DEFAULT);

	void MoveUp(Vec3 &pos);
	void MoveDown(Vec3 &pos);
	Vec3 MovePushed(Vec3 pos);

	//Rotate in relation to given vector plus an offset (angle)
	void RotateRelativeVec3(Vec3 &rot, Vec3 relativeVec, float angleY);
	
	//Rotate the different vectors in Orientation
	void RotateX(Vec3 &rot, float angleX);
	void RotateY(Vec3 &rot, float angleY);
	void RotateZ(Vec3 &rot, float angleZ);
	void ResetRot(Vec3 &rot);

	//Set parameters of calculations, achieveing different effects. And other setfunctions
	void SetGravity(Vec3 gravity)						{ this->mGravity = gravity; }
	void SetMass(float mass)							{ this->mMass = mass; }
	void SetVelocity(Vec3 vel)							{ this->mVelocity = vel; }
	void SetOrientation(Vec3 look, Vec3 right, Vec3 up) { this->mOrient.SetOrientation(look, right, up); }
	void SetPushDirection(Vec3 direction)				{ this->mPushDirection = direction; }

	//Standard getfunctions.
	float GetMass() const				{ return this->mMass; }
	Vec3 GetGravity() const				{ return this->mGravity; }
	PhysicsEntityStates* GetStates()	{ return this->mStates; } 
	Orientation GetOrientation() const	{ return this->mOrient; }
	Vec3 GetVelocity() const			{ return this->mVelocity; }//Fetched from EntityManager (used for checking if you can push something)

	//Set gravity that should affect all the entities
	static void SetGlobalGravity(Vec3 gravity) { PhysicsEntityImpl::mGlobalGravity = gravity; }

private:
	float Lerp(float a, float b, float amount);
};


#endif