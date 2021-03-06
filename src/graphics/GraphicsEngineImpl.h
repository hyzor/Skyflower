#ifndef GRAPHICS_GRAPHICSENGINEIMPL_H
#define GRAPHICS_GRAPHICSENGINEIMPL_H

#include "shared/platform.h"
#include "Direct3D.h"
#include "GraphicsEngine.h"
#include "GenericModel.h"
#include "GenericSkinnedModel.h"
#include "TextureManager.h"
#include "Camera.h"
#include "CameraControllImpl.h"
#include <map>
#include <string>
#include "Sky.h"
#include "Shadowmap.h"
#include "CascadedShadows.h"
#include "RenderStates.h"
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>

#include "GraphicsEngine.h"
#include "InstanceImpl.h"

#include "DeferredBuffers.h"
#include "OrthoWindow.h"
#include "Texture2DImpl.h"

#include "ParticleSystemImpl.h"

#include "GenericSkinnedModelSorted.h"

#include "SMAA.h"
#include "External/SMAA/AreaTex.h"
#include "FullscreenTriangle.h"



// const float fovY = 0.785398f; // 0.25f * MathHelper::pi
// const float zNear = 1.0f;
// const float zFar = 1000.0f;

static const enum BlendingMethods
{
	ALPHA_BLENDING = 0,
	ADDITIVE_BLENDING
};

class GraphicsEngineImpl : public GraphicsEngine
{
public:
	GraphicsEngineImpl();
	~GraphicsEngineImpl();

	bool Init(HWND hWindow, UINT width, UINT height, const std::string &resourceDir);
	void OnResize(UINT width, UINT height);

	void Run(float dt);

	void DrawScene();
	void UpdateScene(float dt, float gameTime);
	void RenderSceneToTexture();
	void Present();

	void Begin2D();
	void End2D();
	void Draw2DTextureFile(const std::string file, const Draw2DInput* input);
	void Draw2DTexture(Texture2D *texture, const Draw2DInput* input);

	ModelInstance* CreateInstance(std::string file);
	ModelInstance* CreateInstance(std::string file, Vec3 pos);
	CameraController *CreateCameraController();
	void DeleteCameraController(CameraController *controller);
	void DeleteInstance(ModelInstance* mi);

	AnimatedInstance* CreateAnimatedInstance(std::string file);
	void DeleteInstance(AnimatedInstance* ai);

	SortedAnimatedInstance* CreateSortedAnimatedInstance(std::string file);
	void DeleteInstance(SortedAnimatedInstance* ai);

	MorphModelInstance* CreateMorphAnimatedInstance(std::string path, std::string file, Vec3 pos);
	void DeleteInstance(MorphModelInstance* mmi);

	Texture2D *CreateTexture2D(unsigned int width, unsigned int height, bool renderable = false);
	void DeleteTexture2D(Texture2D *texture);

	ParticleSystem *CreateParticleSystem();
	void DeleteParticleSystem(ParticleSystem *particleSystem);

	//void CreateMorphInstance(std::string file);

	void UpdateSceneData();

	DirectionalLight* AddDirLight(Vec3 color, Vec3 direction, float intensity);
	PointLight* AddPointLight(Vec3 color, Vec3 Position, float intensity);
	SpotLight* AddSpotLight(Vec3 color, Vec3 direction, Vec3 Position, float angle);

	void ClearLights();

	void PrintText(std::string text, int x, int y, Vec3 color = Vec3::Zero(), float scale = 1.0f, float alpha = 1.0f);
	Vec3 MeassureString(const std::string text);
	void Clear();

	void SetFullscreen(bool fullscreen);
	bool IsFullscreen();
	void GetWindowResolution(UINT& width, UINT& height);

	unsigned int GetPostProcessingEffects();
	void SetPostProcessingEffects(unsigned int effects);
	void SetDepthOfFieldFocusPlanes(float nearBlurryPlane, float nearSharpPlane, float farSharpPlane, float farBlurryPlane);
	void SetSSAOParameters(float radius, float projection_factor, float bias, float contrast, float sigma);
	void SetMorphAnimWeigth(unsigned index, Vec3 weight);
	Vec3 GetMorphAnimWeigth(unsigned index);

	void ClearTexture(Texture2D *texture, const float color[4]);
	void PrintTextMonospaceToTexture(Texture2D *texture, const std::string &text, const int position[2]);
	void DrawLines(Texture2D *texture, const float *data, size_t count, const XMFLOAT3X3 &transformation, const float color[4]);

	void SetFogProperties(int enableFogging, float heightFalloff, float heightOffset, float globalDensity, XMFLOAT4 fogColor);

	void ResetRenderTargetAndViewport();

	void ClearLevelTextures();
	void SetSkyTexture(const std::string& fileName);

	void ClearModelInstances();

private:
	Direct3D* mD3D;

	std::string mResourceDir;

	TextureManager* mTextureMgr;

	Camera* mCamera;

	InputLayouts* mInputLayouts;


	std::map<std::string, GenericModel*> mModels;
	std::map<std::string, GenericSkinnedModel*> mSkinnedModels;
	std::map<std::string, GenericSkinnedModelSorted*> mSkinnedSortedModels;

	std::vector<ModelInstanceImpl*> mInstances;
	std::vector<AnimatedInstanceImpl*> mAnimatedInstances;
	std::vector<SortedAnimatedInstanceImpl*> mSortedAnimatedInstances;

	std::vector<MorphModel*> mMorphModels;
	std::vector<MorphModelInstanceImpl*> mMorphInstances;

	PLight* mPointLights;
	DLight* mDirLights;
	SLight* mSpotLights;

	int mPointLightsCount;
	int mDirLightsCount;
	int mSpotLightsCount;

	Sky* mSky;
	CascadedShadows* mCascadedShadows;
	float mShadowFrustumCalcTimer;

	ShaderHandler* mShaderHandler;

	SpriteBatch* mSpriteBatch;
	SpriteFont* mSpriteFont;
	SpriteFont* mSpriteFontMonospace;

	DirectX::BoundingSphere mSceneBounds;
	DirectX::BoundingBox mSceneBB;

	DeferredBuffers* mDeferredBuffers;
	OrthoWindow* mOrthoWindow;

	Texture2DImpl *mIntermediateTexture;

	float mSSAOScale;
	Texture2DImpl *mSSAOTexture;
	Texture2DImpl *mSSAOBlurTexture;
	float mSSAOradius;
	float mSSAOprojectionFactor;
	float mSSAObias;
	float mSSAOcontrast;
	float mSSAOsigma;

	float mDoFScale;
	Texture2DImpl *mDoFCoCTexture;
	Texture2DImpl *mDoFBlurTexture1;
	Texture2DImpl *mDoFBlurTexture2;
	float mNearBlurryPlane, mNearSharpPlane, mFarSharpPlane, mFarBlurryPlane;

	float morphTimeTest;
	bool morphIncrease;

	unsigned int mPostProcessingEffects;

	float mGameTime;

	std::vector<ParticleSystemImpl*> mParticleSystems;
	ID3D11ShaderResourceView* mRandom1DTexSRV;
	ID3D11ShaderResourceView* mParticlesTextureArray;

	void LoadParticles(std::string particlesPath, std::string particleFileName);

	float mCurFPS;
	float mTargetFPS;

	

	// Keep a list of all the materials used in a specific level
	std::vector<Material*> mMaterials;

	SMAA* mSMAA;

	FullscreenTriangle* mFullscreenTriangle;

	bool mEnableAntiAliasing;

	ID3D11Texture2D* mDepthStencilTextureCopy;
	ID3D11ShaderResourceView* mDepthStencilSRVCopy;

	//GenericSkinnedModelSorted* mTestSortedModel;

	size_t mLineVertexBufferSize;
	ID3D11Buffer *mLineVertexBuffer;

	UINT mScreenWidth;
	UINT mScreenHeight;
};

#endif
