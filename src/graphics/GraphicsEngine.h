#ifndef GRAPHICS_GRAPHICSENGINE_H
#define GRAPHICS_GRAPHICSENGINE_H

#include "shared/platform.h"
#include "shared/Vec3.h"
#include <string>

//DLL_API void InitGraphicsEngine();


class DLL_API ModelInstance
{
public:
	std::string model;
	bool isVisible;
	Vec3 pos;
	Vec3 rot;
	Vec3 scale;
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
	virtual void DeleteInstance(ModelInstance* mi) = 0;

	//virtual bool HasDevice();
	//virtual void GetFullscreenState(BOOL* fullscreenVariable);
};

DLL_API GraphicsEngine* CreateGraphicsEngine();
DLL_API void DestroyGraphicsEngine(GraphicsEngine* engine);


#endif
