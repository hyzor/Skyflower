#include "CameraController.h"
#include "CameraControllImpl.h"

// Must be included last!
#include "shared/debug.h"

DLL_API CameraController* CreateCameraControll(Camera *c)
{
	CameraController* cameraController = (CameraController*) new CameraControllImpl(c);

	return cameraController;
}

DLL_API void DestroyCameraController(CameraController* cameraController)
{
	if (cameraController)
		delete cameraController;
}