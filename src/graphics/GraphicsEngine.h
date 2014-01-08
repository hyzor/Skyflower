#ifndef GRAPHICS_GRAPHICSENGINE_H
#define GRAPHICS_GRAPHICSENGINE_H

#include "CameraController.h"
#include "Instance.h"
#include <string>


class DLL_API GraphicsEngine
{
public:
	virtual ~GraphicsEngine() {}

	virtual bool Init(HWND hWindow, UINT width, UINT height, const std::string &resourceDir) = 0;
	virtual void Run(float dt) = 0;
	virtual void DrawScene() = 0;
	virtual void UpdateScene(float dt) = 0;
	virtual void OnResize(UINT width, UINT height) = 0;

	virtual ModelInstance* CreateInstance(std::string file) = 0;
	virtual ModelInstance* CreateInstance(std::string file, Vec3 pos) = 0;
	virtual void DeleteInstance(ModelInstance* mi) = 0;

	virtual AnimatedInstance* CreateAnimatedInstance(std::string file) = 0;
	virtual void DeleteInstance(AnimatedInstance* ai) = 0;


	virtual CameraController *CreateCameraController() = 0;
};

DLL_API GraphicsEngine* CreateGraphicsEngine();
DLL_API void DestroyGraphicsEngine(GraphicsEngine* engine);


#endif
