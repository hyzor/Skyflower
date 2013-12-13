#ifndef GRAPHICS_GRAPHICSENGINEIMPL_H
#define GRAPHICS_GRAPHICSENGINEIMPL_H

#include "shared/platform.h"
#include "Direct3D.h"
#include "GraphicsEngine.h"
#include "Effects.h"
#include "GenericModel.h"
#include "GenericSkinnedModel.h"
#include "TextureManager.h"
#include "Camera.h"
#include "CameraControllImpl.h"
#include <map>
#include <string>
#include "Sky.h"

//#include "AnimatedEntity.h"
#include "Character.h"
#include "GraphicsEngine.h"
#include "InstanceImpl.h"



class GraphicsEngineImpl : public GraphicsEngine
{
public:
	GraphicsEngineImpl();
	~GraphicsEngineImpl();

	bool Init(HWND hWindow, int width, int height, const std::string &resourceDir);
	int Run();
	//int DrawScene();

	void DrawScene();
	void UpdateScene(float dt);

	ModelInstance* CreateInstance(std::string file);
	ModelInstance* CreateInstance(std::string file, Vec3 pos);
	CameraController *CreateCameraController();
	void DeleteInstance(ModelInstance* mi);

	AnimatedInstance* CreateAnimatedInstance(std::string file);
	void DeleteInstance(AnimatedInstance* ai);

	void OnResize();

	//bool HasDevice() { return mDirect3D->GetDevice() != NULL; }
	//void GetFullscreenState(BOOL* fullscreenVariable) { mDirect3D->GetSwapChain()->GetFullscreenState(fullscreenVariable, NULL); }

private:
	Direct3D* mDirect3D;
	//D3dWindow* mD3dWindow;

	std::string mResourceDir;

	TextureManager* mTextureMgr;
	GenericModel* mDuckModel;
	GenericSkinnedModel* mCharacterModel;

	Camera* mCamera;

	Effects* mEffects;

	std::map<std::string, GenericModel*> mModels;
	std::map<std::string, GenericSkinnedModel*> mSkinnedModels;

	std::vector<ModelInstanceImpl*> mInstances;
	std::vector<AnimatedInstanceImpl*> mAnimatedInstances;
	//std::vector<GenericSkinnedModelInstance> mSkinnedInstances;

	AnimatedEntity* mAnimatedEntity;


	DirectionalLight mDirLights[3];

	Character* mCharacter;

	MSG msg;

	Sky* mSky;
};

#endif

