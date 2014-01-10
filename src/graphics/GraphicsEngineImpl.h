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
#include "RenderStates.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>

#include "Character.h"
#include "GraphicsEngine.h"

class GraphicsEngineImpl : public GraphicsEngine
{
public:
	GraphicsEngineImpl();
	~GraphicsEngineImpl();

	bool Init(HWND hWindow, int width, int height, const std::string &resourceDir);
	void Run(float dt);

	void DrawScene();
	void UpdateScene(float dt);

	ModelInstance* CreateInstance(std::string file);
	ModelInstance* CreateInstance(std::string file, Vec3 pos);
	CameraController *CreateCameraController();
	void DeleteInstance(ModelInstance* mi);

	AnimatedInstance* CreateAnimatedInstance(std::string file);
	void DeleteInstance(AnimatedInstance* ai);

	void OnResize();
	void UpdateSceneData();

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

	Sky* mSky;
	ShadowMap* mShadowMap;

	ShaderHandler* mShaderHandler;

	SpriteBatch* mSpriteBatch;
	SpriteFont* mSpriteFont;

	DirectX::BoundingSphere mSceneBounds;
};

#endif

