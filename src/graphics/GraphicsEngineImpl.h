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
#include "ShaderHandler.h"
#include "RenderStates.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>

#include "Character.h"
#include "GraphicsEngine.h"
#include "InstanceImpl.h"

#include "DeferredBuffers.h"
#include "OrthoWindow.h"

const float zNear = 1.0f;
const float zFar = 10000.0f;

class GraphicsEngineImpl : public GraphicsEngine
{
public:
	GraphicsEngineImpl();
	~GraphicsEngineImpl();

	bool Init(HWND hWindow, UINT width, UINT height, const std::string &resourceDir);
	void Run(float dt);

	void DrawScene();
	void UpdateScene(float dt);
	void RenderSceneToTexture();

	ModelInstance* CreateInstance(std::string file);
	ModelInstance* CreateInstance(std::string file, Vec3 pos);
	CameraController *CreateCameraController();
	void DeleteInstance(ModelInstance* mi);

	AnimatedInstance* CreateAnimatedInstance(std::string file);
	void DeleteInstance(AnimatedInstance* ai);

	void OnResize(UINT width, UINT height);

private:
	Direct3D* mD3D;

	std::string mResourceDir;

	TextureManager* mTextureMgr;

	Camera* mCamera;

	InputLayouts* mInputLayouts;

	std::map<std::string, GenericModel*> mModels;
	std::map<std::string, GenericSkinnedModel*> mSkinnedModels;

	std::vector<ModelInstanceImpl*> mInstances;
	std::vector<AnimatedInstanceImpl*> mAnimatedInstances;

	std::vector<PointLight> mPointLights;
	std::vector<DirectionalLight> mDirLights;
	std::vector<SpotLight> mSpotLights;

	Sky* mSky;
	ShadowMap* mShadowMap;

	ShaderHandler* mShaderHandler;

	SpriteBatch* mSpriteBatch;
	SpriteFont* mSpriteFont;

	DeferredBuffers* mDeferredBuffers;
	OrthoWindow* mOrthoWindow;
};

#endif

