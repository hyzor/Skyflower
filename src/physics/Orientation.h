#ifndef ORIENTATION_H
#define ORIENTATION_H

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
public:
	Orientation();
	Orientation(const Orientation &other);
	virtual ~Orientation();

	void update(float deltaTime);
	void strafe(float length, Vec3 &pos);
	void walk(float length, Vec3 &pos);
	void rotateX(Vec3 &rot, float angle);
	void rotateY(Vec3 &rot, float angle);
	void rotateZ(Vec3 &rot, float angle);
	void rotateAxis(Vec3 &rot, Vec3 axis, float angle);
	void resetRotationXYZ(Vec3 &rot, int Axis); // the enum "Axis"

	Vec3 getLook() const;
	Vec3 getRight() const;
	Vec3 getUp() const;
};

#endif