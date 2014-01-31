#ifndef PHYSICS_PHYSICSENTITY_H
#define PHYSICS_PHYSICSENTITY_H


#include "shared/platform.h"
#include "shared/Vec3.h"
#include "PhysicsHelper.h"
#include "Orientation.h"


struct PhysicsEntityStates
{
	bool isJumping;
	bool isMoving;
	bool isBeingPushed;
	bool isActiveProjectile;
};

//Currently sort of a helper class, to be held by each entity to make sure that the velocity and other entity-specific data are kept by and manipulated only by the right entity
class PhysicsEntity
{
public:
	virtual ~PhysicsEntity(){};

	//Call each frame, updates delta and orientation
	virtual void Update(float dt) = 0;

	//perform a jump on the given vector that represents a position by increasing velocity in Y-axis
	virtual bool Jump(Vec3 &pos) = 0; 

	// Dessa gör mer än att bara applicera gravitationen, utan att kalla på någon av dessa
	// funktioner fungerar ingen velocity alls, alltså kan man inte hoppa om man inte kallar
	// någon av dessa funktion. Därav den dumma addGravity boolen.
	//apply gravity the given vector that represents a position
	virtual void AddGravityCalc(Vec3 &pos, Vec3 &velocity, bool addGravity) = 0; 
	virtual void AddGravityCalc(Vec3 &pos, bool addGravity) = 0;

	//to be used for projectile calculations
	virtual bool FireProjectile(Vec3 &pos, Vec3 direction) = 0;
	virtual bool FireProjectileAt(Vec3 &pos, Vec3 target) = 0;

	//walk along the look vector kept in Orientation
	virtual void Walk(Vec3 &pos, float speed) = 0;
	virtual void Walk(Vec3 &pos) = 0;

	//walk along the right vector kept in Orientation
	virtual void Strafe(Vec3 &pos, float speed) = 0;
	virtual void Strafe(Vec3 &pos) = 0;

	virtual void MoveUp(Vec3 &pos) = 0;
	virtual void MoveDown(Vec3 &pos) = 0;

	virtual Vec3 MovePushed(Vec3 pos) = 0;

	//rotate in relation to given vector plus an offset (angle) and move
	virtual void MoveRelativeVec3(Vec3 &pos, Vec3 &relativeVec, Vec3 &rot, float angleY) = 0;
	virtual void MoveRelativeVec3(Vec3 &pos, Vec3 &relativeVec, float speed,Vec3 &rot, float angleY) = 0;
	
	//rotate the different vectors in Orientation
	virtual void RotateX(Vec3 &rot, float angleX) = 0;
	virtual void RotateY(Vec3 &rot, float angleY) = 0;
	virtual void RotateZ(Vec3 &rot, float angleZ) = 0;
	virtual void ResetRot(Vec3 &rot) = 0;

	//set parameters of calculations, achieveing different effects. And other setfunctions
	virtual void SetGravity(Vec3 gravity) = 0;
	virtual void SetMass(float mass) = 0;
	virtual void SetVelocity(Vec3 vel) = 0;
	virtual void SetOrientation(Vec3 look, Vec3 right, Vec3 up) = 0;
	virtual void SetPushDirection(Vec3 direction) = 0;

	//Standard getfunctions.
	virtual float GetMass() const = 0;
	virtual Vec3 GetGravity() const = 0;
	virtual PhysicsEntityStates* GetStates() = 0; //Not constant,
	virtual Orientation GetOrientation() const = 0;
	virtual Vec3 GetVelocity() const = 0; //Fetched from EntityManager (used for checking if you can push something)

	//Set gravity that should affect all the entities
	//static void SetGlobalGravity(Vec3 gravity);

private:
	virtual float Lerp(float a, float b, float amount) = 0;
};


#endif