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
#include "CameraControllImpl.h"
#include <map>
#include <string>
#include "Sky.h"

//#include "AnimatedEntity.h"
#include "Character.h"
#include "GraphicsEngine.h"

class DLL_API ModelInstanceImpl : public ModelInstance
{
public:
	ModelInstanceImpl(Vec3 pos, Vec3 rot, Vec3 scale);
	~ModelInstanceImpl();

	bool IsVisible();
	Vec3 GetPosition();
	Vec3 GetRotation();
	Vec3 GetScale();

	void SetVisibility(bool visible);
	void SetPosition(Vec3 pos);
	void SetRotation(Vec3 rot);
	void SetScale(Vec3 scale);
	void Set(Vec3 pos, Vec3 rot, Vec3 scale);

	XMMATRIX GetWorld();

	GenericModel* model;

private:
	XMFLOAT4X4 modelOffset;
	XMFLOAT4X4 modelRot;
	XMFLOAT4X4 modelScale;
	XMFLOAT4X4 modelWorld;

	bool isVisible;
	Vec3 pos;
	Vec3 rot;
	Vec3 scale;

};


DLL_API void InitGraphicsEngine();

class DLL_API GraphicsEngineImpl : public GraphicsEngine
{
public:
	GraphicsEngineImpl();
	~GraphicsEngineImpl();

	bool Init(HWND hWindow, int width, int height);
	int Run();
	//int DrawScene();

	void DrawScene();
	void UpdateScene(float dt);

	ModelInstance* CreateInstance(std::string file);
	ModelInstance* CreateInstance(std::string file, Vec3 pos);
	CameraController *CreateCameraController();
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

	std::vector<ModelInstanceImpl*> mInstances;
	//std::vector<GenericSkinnedModelInstance> mSkinnedInstances;

	AnimatedEntity* mAnimatedEntity;


	DirectionalLight mDirLights[3];

	Character* mCharacter;

	MSG msg;

	Sky* mSky;
};

#endif

