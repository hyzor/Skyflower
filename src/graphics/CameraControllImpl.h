#ifndef CAMERACONTROLLIMPL_h
#define CAMERACONTROLLIMPL_h

#include "CameraController.h"
#include "GraphicsEngine.h"
#include "Camera.h"

class DLL_API CameraControllImpl : CameraController
{
public:
	CameraControllImpl(Camera *c);
	virtual ~CameraControllImpl();
	 void SetPosition(Vec3 position);
	 void SetDirection(Vec3 direction);
	 void SetOffset(float offset);
	 void Rotate(Vec3 rot);
	 void Update();
	 void Follow(Vec3 target);
	 void RotateCamera(float mouseX, float mouseY);
	 Vec3 GetPosition();
	 Vec3 GetDirection();
private:
	Camera* camera;
	Vec3 o;
	Vec3 target;
	float offset;

	float rot;
	float rotx;

	Vec3 lookat;
};

DLL_API CameraController* CreateCameraControll(Camera *c);
DLL_API void DestroyCameraController(CameraController* cameraController);

#endif