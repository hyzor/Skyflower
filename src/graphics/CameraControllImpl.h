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
	 void Rotate(Vec3 rot);

	 Vec3 GetPosition();
	 Vec3 GetDirection();
private:
	Camera* camera;
};

DLL_API CameraController* CreateCameraControll(Camera *c);
DLL_API void DestroyCameraController(CameraController* cameraController);

#endif