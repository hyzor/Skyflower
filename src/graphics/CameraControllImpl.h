#ifndef CAMERACONTROLLIMPL_h
#define CAMERACONTROLLIMPL_h

#include "CameraController.h"
#include "GraphicsEngine.h"
#include "Camera.h"

#define MAX_ZOOM 50
#define MIN_ZOOM 100

class DLL_API CameraControllImpl : CameraController
{
public:
	CameraControllImpl(Camera *c);
	virtual ~CameraControllImpl();
	 void SetPosition(Vec3 position);
	 void SetDirection(Vec3 direction);
	 void SetOffset(float offset);
	 void Rotate(Vec3 rot);
	 void Update(float dt);
	 void Follow(Vec3 target);
	 void RotateCamera(float mouseX, float mouseY);
	 void Zoom(float d, float speed);
	 Vec3 GetPosition();
	 Vec3 GetDirection();
	 Vec3 GetLook();
	 Vec3 GetRight();
	 Vec3 GetUp();
private:
	Camera* camera;
	Vec3 o;
	Vec3 target;
	float offset;

	float yaw;
	float pitch;
	float targetPitch;
	float targetYaw;
	float rot;
	float rotx;

	Vec3 lookat;
};

DLL_API CameraController* CreateCameraControll(Camera *c);
DLL_API void DestroyCameraController(CameraController* cameraController);

#endif