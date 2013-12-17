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
	virtual void Update(float dt) = 0;
	virtual void Follow(Vec3 target) = 0;
	virtual Vec3 GetPosition() = 0;
	virtual Vec3 GetDirection() = 0;
	virtual Vec3 GetLook() = 0;
	virtual Vec3 GetRight() = 0;
	virtual Vec3 GetUp() = 0;
	virtual void RotateCamera(float mouseX, float mouseY) = 0;
	virtual void Zoom(float, float speed) = 0;
};

#endif