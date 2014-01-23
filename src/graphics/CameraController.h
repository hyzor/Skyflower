/*Interface for the camera controller.
  Any function that can be used by the camera controller
  implementation must be here.
*/
#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include "shared/Vec3.h"

class DLL_API CameraController
{
public:
	virtual ~CameraController() {}
	virtual void SetPosition(Vec3 pos) = 0;
	virtual void SetDirection(Vec3 dir) = 0;
	virtual void SetOffset(float offset) = 0; // Set the offset from the player.
	virtual void Update(float dt) = 0;
	virtual void Follow(Vec3 target) = 0; // Set target for the camera to look at and follow
	virtual Vec3 GetPosition() = 0;
	virtual Vec3 GetDirection() = 0;
	virtual Vec3 GetLook() = 0; // Look vector
	virtual Vec3 GetRight() = 0; // Right vector
	virtual Vec3 GetUp() = 0; // Up vector
	virtual float GetYaw() = 0;
	virtual float GetPitch() = 0;
	virtual void RotateCamera(float mouseX, float mouseY) = 0;
	virtual void Zoom(float, float speed) = 0; // Lowers/Increases the offset to the player
};

DLL_API void DestroyCameraController(CameraController* cameraController); // Clean up
#endif