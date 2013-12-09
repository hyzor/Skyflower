#ifndef GRAPHICS_GRAPHICSENGINEIMPL_H
#define GRAPHICS_GRAPHICSENGINEIMPL_H

#include "shared/platform.h"
#include "Direct3D.h"
#include "D3dWindow.h"
#include "GraphicsEngine.h"
#include "Effects.h"
#include "GenericModel.h"
#include "GenericSkinnedModel.h"
#include "TextureManager.h"
#include "Camera.h"
#include <map>
#include <string>
#include "Sky.h"

//#include "AnimatedEntity.h"
#include "Character.h"

DLL_API void InitGraphicsEngine();

class DLL_API GraphicsEngineImpl : public GraphicsEngine
{
public:
	GraphicsEngineImpl();
	~GraphicsEngineImpl();

	bool Init(HWND hWindow);
	int Run();
	//int DrawScene();

	void DrawScene();
	void UpdateScene(float dt);

	ModelInstance* CreateInstance(std::string file);
	void DeleteInstance(ModelInstance* mi);

	void OnResize();

	//bool HasDevice() { return mDirect3D->GetDevice() != NULL; }
	//void GetFullscreenState(BOOL* fullscreenVariable) { mDirect3D->GetSwapChain()->GetFullscreenState(fullscreenVariable, NULL); }

private:
	Direct3D* mDirect3D;
	//D3dWindow* mD3dWindow;

	TextureManager* mTextureMgr;
	GenericModel* mDuckModel;
	GenericSkinnedModel* mCharacterModel;

	Camera* mCamera;

	Effects* mEffects;

	std::map<std::string, GenericModel*> mModels;
	std::map<std::string, GenericSkinnedModel*> mSkinnedModels;

	std::vector<ModelInstance*> mInstances;
	//std::vector<GenericSkinnedModelInstance> mSkinnedInstances;

	AnimatedEntity* mAnimatedEntity;

	ModelInstance mDuckInstance;

	DirectionalLight mDirLights[3];

	Character* mCharacter;

	MSG msg;

	Sky* mSky;
};

#endif

