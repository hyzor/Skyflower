#ifndef GRAPHICS_GRAPHICSENGINE_H
#define GRAPHICS_GRAPHICSENGINE_H

#include "CameraController.h"
#include "Instance.h"
#include <string>


class DLL_API GraphicsEngine
{
public:
	virtual ~GraphicsEngine() {}

	virtual bool Init(HWND hWindow, int width, int height, const std::string &resourceDir) = 0;
	virtual int Run() = 0;
	virtual void DrawScene() = 0;
	virtual void UpdateScene(float dt) = 0;

	virtual ModelInstance* CreateInstance(std::string file) = 0;
	virtual ModelInstance* CreateInstance(std::string file, Vec3 pos) = 0;
	virtual void DeleteInstance(ModelInstance* mi) = 0;

	virtual AnimatedInstance* CreateAnimatedInstance(std::string file) = 0;
	virtual void DeleteInstance(AnimatedInstance* ai) = 0;


	virtual CameraController *CreateCameraController() = 0;
	//virtual Camera* GetCameraPtr() = 0;
	//virtual bool HasDevice();
	//virtual void GetFullscreenState(BOOL* fullscreenVariable);
};

DLL_API GraphicsEngine* CreateGraphicsEngine();
DLL_API void DestroyGraphicsEngine(GraphicsEngine* engine);


#endif
