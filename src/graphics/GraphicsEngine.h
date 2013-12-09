#ifndef GRAPHICS_GRAPHICSENGINE_H
#define GRAPHICS_GRAPHICSENGINE_H

#include "CameraController.h"
#include "shared/platform.h"
#include "shared/Vec3.h"
#include <string>

//DLL_API void InitGraphicsEngine();


class DLL_API ModelInstance
{
public:
	virtual ~ModelInstance() {};

	virtual bool IsVisible() = 0;
	virtual Vec3 GetPosition() = 0;
	virtual Vec3 GetRotation() = 0;
	virtual Vec3 GetScale() = 0;

	virtual void SetPosition(Vec3 pos) = 0;
	virtual void SetRotation(Vec3 rot) = 0;
	virtual void SetScale(Vec3 scale) = 0;
	virtual void SetVisibility(bool visible) = 0;
	virtual void Set(Vec3 pos, Vec3 rot, Vec3 scale) = 0;

};

class DLL_API GraphicsEngine
{
public:
	virtual ~GraphicsEngine() {}

	virtual bool Init(HWND hWindow) = 0;
	virtual int Run() = 0;
	virtual void DrawScene() = 0;
	virtual void UpdateScene(float dt) = 0;

	virtual ModelInstance* CreateInstance(std::string file) = 0;
	virtual ModelInstance* CreateInstance(std::string file, Vec3 pos) = 0;
	virtual void DeleteInstance(ModelInstance* mi) = 0;

	virtual CameraController *CreateCameraController() = 0;
	//virtual Camera* GetCameraPtr() = 0;
	//virtual bool HasDevice();
	//virtual void GetFullscreenState(BOOL* fullscreenVariable);
};

DLL_API GraphicsEngine* CreateGraphicsEngine();
DLL_API void DestroyGraphicsEngine(GraphicsEngine* engine);


#endif
