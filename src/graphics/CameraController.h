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
	virtual Vec3 GetPosition() const = 0;
	virtual Vec3 GetLook() = 0; // Look vector
	virtual Vec3 GetRight() const = 0; // Right vector
	virtual Vec3 GetUp() const = 0; // Up vector
	virtual float GetYaw() const = 0;
	virtual float GetPitch() const = 0;
	virtual void onMouseMove(float mouseX, float mouseY) = 0;
	virtual void Rotate(float yaw, float pitch) = 0;
	virtual void Zoom(float, float speed) = 0; // Lowers/Increases the offset to the player
	virtual void SetInverted(bool invert) = 0;
	virtual bool GetInverted() const = 0;
	virtual void SetMouseSense(float sensitivity) = 0;
	virtual float GetMouseSense() const = 0;
};

DLL_API void DestroyCameraController(CameraController* cameraController); // Clean up
#endif