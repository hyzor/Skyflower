#ifndef ORIENTATION_H
#define ORIENTATION_H

#include "shared/platform.h"
#include "Vec3.h"
#include <DirectXMath.h>

class Orientation
{
private:
	Vec3 position;
	Vec3 right;
	Vec3 up;
	Vec3 look;
public:
	Orientation();
	Orientation(const Orientation &other);
	virtual ~Orientation();

	void update(float deltaTime);
	void strafe(float length);
	void walk(float length);
	void rotateY(float angle);
	void rotateX(float angle);
	void rotateAxis(Vec3 axis, float angle);
};

#endif