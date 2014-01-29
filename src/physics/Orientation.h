#ifndef PHYSICS_ORIENTATION_H
#define PHYSICS_ORIENTATION_H

#include "shared/platform.h"
#include "shared/Vec3.h"
#include <DirectXMath.h>

enum Axis
{
	X = 0,
	Y,
	Z
};

class DLL_API Orientation
{
private:
	Vec3 right;
	Vec3 up;
	Vec3 look;
	Vec3 rotRelativeCam;
public:
	Orientation();
	Orientation(const Orientation &other);
	virtual ~Orientation();

	void Update(float deltaTime);
	void Strafe(float length, Vec3 &pos);
	void Walk(float length, Vec3 &pos);
	void RotateX(Vec3 &rot, float angle);
	void RotateY(Vec3 &rot, float angle);
	void RotateZ(Vec3 &rot, float angle);
	void RotateAxis(Vec3 &rot, Vec3 axis, float angle);
	void ResetRotationXYZ(Vec3 &rot, int Axis); // the enum "Axis"

	void SetOrientation(Vec3 look, Vec3 right, Vec3 up);
	void SetRotRelativeCam(Vec3 rot);

	Vec3 GetLook() const;
	Vec3 GetRight() const;
	Vec3 GetUp() const;
	Vec3 GetRotRelativeCam() const;
};

#endif