#ifndef GRAPHICS_GRAPHICSENGINE_H
#define GRAPHICS_GRAPHICSENGINE_H

#include "CameraController.h"
#include "Instance.h"
#include "LightDef.h"
#include "Texture2D.h"
#include "ParticleSystem.h"
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
	virtual void UpdateScene(float dt, float gameTime) = 0;
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

	virtual SortedAnimatedInstance* CreateSortedAnimatedInstance(std::string file) = 0;
	virtual void DeleteInstance(SortedAnimatedInstance* ai) = 0;

	virtual MorphModelInstance* CreateMorphAnimatedInstance(std::string path, std::string file, Vec3 pos) = 0;
	virtual void DeleteInstance(MorphModelInstance* mmi) = 0;

	virtual DirectionalLight* AddDirLight(Vec3 color, Vec3 direction, float intensity) = 0;
	virtual PointLight* AddPointLight(Vec3 color, Vec3 Position, float intensity) = 0;
	virtual SpotLight* AddSpotLight(Vec3 color, Vec3 direction, Vec3 Position, float angle) = 0;
	virtual void ClearLights() = 0;

	virtual CameraController *CreateCameraController() = 0;
	virtual void DeleteCameraController(CameraController *controller) = 0;

	virtual void UpdateSceneData() = 0;
	virtual Texture2D *CreateTexture2D(unsigned int width, unsigned int height, bool renderable = false) = 0;
	virtual void DeleteTexture2D(Texture2D *texture) = 0;

	virtual ParticleSystem *CreateParticleSystem() = 0;
	virtual void DeleteParticleSystem(ParticleSystem *particleSystem) = 0;

	virtual void PrintText(std::string text, int x, int y, Vec3 color = Vec3::Zero(), float scale = 1.0f, float alpha = 1.0f) = 0;
	virtual Vec3 MeassureString(const std::string text) = 0;
//	virtual Text2D* CreateText2D() = 0;
//	virtual void DeleteTexture2D(Text2D *text) = 0;

	virtual void SetFullscreen(bool fullscreen) = 0;
	virtual bool IsFullscreen() = 0;
	virtual void GetWindowResolution(UINT& width, UINT& height) = 0;
	virtual void Clear() = 0;

	virtual unsigned int GetPostProcessingEffects() = 0;
	virtual void SetPostProcessingEffects(unsigned int effects) = 0;
	virtual void SetDepthOfFieldFocusPlanes(float nearBlurryPlane, float nearSharpPlane, float farSharpPlane, float farBlurryPlane) = 0;
	virtual void SetSSAOParameters(float radius, float projection_factor, float bias, float contrast, float sigma) = 0;
	virtual void SetMorphAnimWeigth(unsigned index, Vec3 weight) = 0;
	virtual Vec3 GetMorphAnimWeigth(unsigned index) = 0;

	virtual void ClearTexture(Texture2D *texture, const float color[4]) = 0;
	virtual void PrintTextMonospaceToTexture(Texture2D *texture, const std::string &text, const int position[2]) = 0;
	virtual void DrawLines(Texture2D *texture, const float *data, size_t count, const XMFLOAT3X3 &transformation, const float color[4]) = 0;

	virtual void SetFogProperties(int enableFogging, float heightFalloff, float heightOffset, float globalDensity, XMFLOAT4 fogColor) =0;

	virtual void ResetRenderTargetAndViewport() = 0;

	virtual void ClearLevelTextures() = 0;
	virtual void SetSkyTexture(const std::string& fileName) = 0;
	virtual void ClearModelInstances() = 0;
};

DLL_API GraphicsEngine* CreateGraphicsEngine();
DLL_API void DestroyGraphicsEngine(GraphicsEngine* engine);


#endif
