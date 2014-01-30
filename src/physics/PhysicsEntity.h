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

class PhysicsEntity
{
public:
	virtual ~PhysicsEntity(){};

	virtual void Update(float dt) = 0;

	virtual bool Jump(Vec3 &pos) = 0; 
	virtual void ApplyVelocityToPos(Vec3 &pos) = 0;
	virtual void AddGravityCalc(Vec3 &pos) = 0;

	virtual bool FireProjectile(Vec3 &pos, Vec3 direction) = 0;
	virtual bool FireProjectileAt(Vec3 &pos, Vec3 target) = 0;

	virtual void Walk(Vec3 &pos, float speed = MOVEMENTSPEED_DEFAULT) = 0;
	virtual void Strafe(Vec3 &pos, float speed = MOVEMENTSPEED_DEFAULT) = 0;

	virtual void MoveUp(Vec3 &pos) = 0;
	virtual void MoveDown(Vec3 &pos) = 0;
	virtual Vec3 MovePushed(Vec3 pos) = 0;

	virtual void MoveRelativeVec3(Vec3 &pos, Vec3 &relativeVec, Vec3 &rot, float angleY) = 0;
	virtual void MoveRelativeVec3(Vec3 &pos, Vec3 &relativeVec, float speed,Vec3 &rot, float angleY) = 0;
	
	virtual void RotateX(Vec3 &rot, float angleX) = 0;
	virtual void RotateY(Vec3 &rot, float angleY) = 0;
	virtual void RotateZ(Vec3 &rot, float angleZ) = 0;
	virtual void ResetRot(Vec3 &rot) = 0;

	virtual void SetGravity(Vec3 gravity) = 0;
	virtual void SetMass(float mass) = 0;
	virtual void SetVelocity(Vec3 vel) = 0;
	virtual void SetOrientation(Vec3 look, Vec3 right, Vec3 up) = 0;
	virtual void SetPushDirection(Vec3 direction) = 0;

	virtual float GetMass() const = 0;
	virtual Vec3 GetGravity() const = 0;
	virtual PhysicsEntityStates* GetStates() = 0;
	virtual Orientation GetOrientation() const = 0;
	virtual Vec3 GetVelocity() const = 0; 

private:
	virtual float Lerp(float a, float b, float amount) = 0;
};


#endif