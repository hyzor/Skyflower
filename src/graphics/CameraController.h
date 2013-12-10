#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include "shared/Vec3.h"

class DLL_API CameraController
{
public:
	virtual ~CameraController() {}
	virtual void SetPosition(Vec3 pos) = 0;
	virtual void SetDirection(Vec3 dir) = 0;
	virtual void SetOffset(float offset) = 0;
	virtual void Rotate(Vec3 rotRads) = 0;
	virtual void Update(float dt, int mouseX, int mouseY) = 0;
	virtual void Follow(Vec3 target) = 0;
	virtual Vec3 GetPosition() = 0;
	virtual Vec3 GetDirection() = 0;
};

#endif