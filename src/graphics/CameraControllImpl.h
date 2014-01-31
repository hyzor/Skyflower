#ifndef CAMERACONTROLLIMPL_h
#define CAMERACONTROLLIMPL_h

#include "CameraController.h"
#include "GraphicsEngine.h"
#include "Camera.h"

#define MAX_ZOOM 20
#define MIN_ZOOM 60

class DLL_API CameraControllImpl : CameraController
{
public:
	CameraControllImpl(Camera *c);
	virtual ~CameraControllImpl();
	 void SetPosition(Vec3 position);
	 void SetDirection(Vec3 direction);
	 void SetOffset(float offset); // Set the offset from the player.
	 void Update(float dt);
	 void Follow(Vec3 target); // Set target for the camera to look at and follow
	 void onMouseMove(float mouseX, float mouseY);
	 void Rotate(float yaw, float pitch);
	 void Zoom(float d, float speed);
	 Vec3 GetPosition(); 
	 Vec3 GetDirection();
	 Vec3 GetLook(); // Look vector
	 Vec3 GetRight(); // Right vector
	 Vec3 GetUp(); // Up vector
	 float GetYaw();
	 float GetPitch();

private:
	Camera* camera;
	Vec3 o; // Vector used for rotation calculations
	Vec3 target; // The target of the camera. Usually the player.
	float offset; // Distance to the target

	float yaw; // Current yaw in radians
	float pitch; // Current pitch in radians
	float targetPitch; // The target pitch. Used in Lerp calculations
	float targetYaw; // The target yaw. Used in Lerp calculations
	float targetY; // The Y-pos of the target. Used for Lerp.
	float targetZoom; 

	float time;
};

DLL_API CameraController* CreateCameraControll(Camera *c);
DLL_API void DestroyCameraController(CameraController* cameraController);

#endif