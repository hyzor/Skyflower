#ifndef GRAPHICS_GRAPHICSENGINE_H
#define GRAPHICS_GRAPHICSENGINE_H

#include "CameraController.h"
#include "Instance.h"
#include "Texture2D.h"
#include <string>
#include "..\shared\Vec3.h"
#include <DirectXMath.h>

enum
{
	POST_PROCESSING_SSAO = (1 << 0),
	POST_PROCESSING_DOF  = (1 << 1),
};

struct Draw2DInput
{
	void* operator new (size_t size)
	{
		void* p = _aligned_malloc(size, 16);

		if (!p)
			throw std::bad_alloc();

		return p;
	}

	void operator delete (void* p)
	{
		Draw2DInput* ptr = static_cast<Draw2DInput*>(p);
		_aligned_free(p);
	}

	DirectX::XMFLOAT2 pos;
	DirectX::XMFLOAT2 scale;
	DirectX::XMFLOAT2 origin;
	DirectX::XMVECTOR color;
	float rot;
	float layerDepth;
};

class DLL_API GraphicsEngine
{
public:
	virtual ~GraphicsEngine() {}

	virtual bool Init(HWND hWindow, UINT width, UINT height, const std::string &resourceDir) = 0;
	virtual void OnResize(UINT width, UINT height) = 0;

	virtual void Run(float dt) = 0;
	virtual void DrawScene() = 0;
	virtual void UpdateScene(float dt) = 0;
	virtual void Present() = 0;

	virtual void Begin2D() = 0;
	virtual void End2D() = 0;
	virtual void Draw2DTextureFile(const std::string file, const Draw2DInput* input) = 0;
	virtual void Draw2DTexture(Texture2D *texture, const Draw2DInput* input) = 0;

	virtual ModelInstance* CreateInstance(std::string file) = 0;
	virtual ModelInstance* CreateInstance(std::string file, Vec3 pos) = 0;
	virtual void DeleteInstance(ModelInstance* mi) = 0;

	virtual AnimatedInstance* CreateAnimatedInstance(std::string file) = 0;
	virtual void DeleteInstance(AnimatedInstance* ai) = 0;

	virtual void addDirLight(Vec3 color, Vec3 direction, float intensity) = 0;
	virtual void addPointLight(Vec3 color, Vec3 Position, float intensity) = 0;
	virtual void addSpotLight(Vec3 color, Vec3 direction, Vec3 Position, float angle) = 0;
	virtual void clearLights() = 0;

	virtual CameraController *CreateCameraController() = 0;
	virtual void DeleteCameraController(CameraController *controller) = 0;

	virtual void UpdateSceneData() = 0;
	virtual Texture2D *CreateTexture2D(unsigned int width, unsigned int height) = 0;
	virtual void DeleteTexture2D(Texture2D *texture) = 0;

	virtual void printText(wchar_t* text, int x, int y, Vec3 color = Vec3::Zero(), float scale = 1.0f) = 0;
//	virtual Text2D* CreateText2D() = 0;
//	virtual void DeleteTexture2D(Text2D *text) = 0;

	virtual void SetFullscreen(bool fullscreen) = 0;
	virtual bool isFullscreen() = 0;
	virtual void Clear() = 0;

	virtual unsigned int GetPostProcessingEffects() = 0;
	virtual void SetPostProcessingEffects(unsigned int effects) = 0;
	virtual void SetDepthOfFieldFocusPlanes(float nearBlurryPlane, float nearSharpPlane, float farSharpPlane, float farBlurryPlane) = 0;
	virtual void SetSSAOParameters(float radius, float projection_factor, float bias, float contrast, float sigma) = 0;
};

DLL_API GraphicsEngine* CreateGraphicsEngine();
DLL_API void DestroyGraphicsEngine(GraphicsEngine* engine);


#endif
