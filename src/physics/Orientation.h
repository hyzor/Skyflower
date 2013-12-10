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
	Vec3 position;
	Vec3 right;
	Vec3 up;
	Vec3 look;
	float rotX;
	float rotY;
	float rotZ;
public:
	Orientation();
	Orientation(const Orientation &other);
	virtual ~Orientation();

	void update(float deltaTime);
	void strafe(float length, Vec3 &pos);
	void walk(float length, Vec3 &pos);
	void rotateY(float angle);
	void rotateX(float angle);
	void rotateAxis(Vec3 axis, float angle);
	void resetRotationXYZ(int Axis); // the enum "Axis"

	Vec3 getPos() const;
	Vec3 getLook() const;
	Vec3 getRight() const;
	Vec3 getUp() const;
	float getRotX() const;
	float getRotY() const;
	float getRotZ() const;

	void setPos(Vec3 pos);
};

#endif