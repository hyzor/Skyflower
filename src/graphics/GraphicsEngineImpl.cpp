#include "GraphicsEngineImpl.h"

GraphicsEngineImpl::GraphicsEngineImpl()
{
}


GraphicsEngineImpl::~GraphicsEngineImpl()
{

	for (auto& it(mModels.begin()); it != mModels.end(); ++it)
	{
		if (it->second)
			SafeDelete(it->second);
	}
	mModels.clear();

	for (unsigned int i = 0; i < mInstances.size(); i++)
	{
		if (mInstances[i])
			delete mInstances[i];
	}

	for (unsigned int i = 0; i < mAnimatedInstances.size(); i++)
	{
		if (mAnimatedInstances[i])
			delete mAnimatedInstances[i];
	}

	for (auto& it(mSkinnedModels.begin()); it != mSkinnedModels.end(); ++it)
	{
		if (it->second)
			SafeDelete(it->second);
	}
	mSkinnedModels.clear();

	//delete mD3dWindow;

	delete mCamera;


	//InputLayouts::DestroyAll();
	mInputLayouts->DestroyAll();
	delete mInputLayouts;

	delete mAnimatedEntity;

	delete mTextureMgr;

	delete mCharacter;

	delete mSky;

	RenderStates::DestroyAll();

	delete mSpriteFont;
	delete mSpriteBatch;

	if (mShaderHandler)
		delete mShaderHandler;

	if (mD3D)
		delete mD3D;
}

bool GraphicsEngineImpl::Init(HWND hWindow, int width, int height, const std::string &resourceDir)
{
	//mD3dWindow = new D3dWindow(hWindow);

	//if (!mD3dWindow)
	//	return false;

	//if (!mD3dWindow->Init())
	//	return false;


	mResourceDir = resourceDir;

	mD3D = new Direct3D();

	mD3D->Init(&hWindow, width, height);

// 	mEffects = new Effects();
// 
// 	mEffects->InitAll(mD3D->GetDevice());
	//InputLayouts::InitAll(mD3D->GetDevice());

	mInputLayouts = new InputLayouts();
	//mInputLayouts->InitAll(mD3D->GetDevice());

	// Shaders
	//Effects::InitAll(mDirect3D->GetDevice());

	RenderStates::InitAll(mD3D->GetDevice());

	mTextureMgr = new TextureManager();

	// Texture manager
	mTextureMgr->Init(mD3D->GetDevice());

	// Load models
	mSkinnedModels["Character"] = new GenericSkinnedModel(mD3D->GetDevice(),
		mTextureMgr,
		mResourceDir + "Models\\Character\\char.dae",
		mResourceDir + "Models\\Character\\");

	// -----------------------------------------
	// Test model instances
	// -----------------------------------------
	mCharacter = new Character(mSkinnedModels["Character"], XMFLOAT3(-20.0f, 0.0f, -200.0f));

	// Create camera
	mCamera = new Camera();

	mCamera->SetLens(0.25f*MathHelper::pi, static_cast<float>(width)/height, 1.0f, 10000.0f);
	mCamera->SetPosition(XMFLOAT3(0.0f, 0.0f, -400.0f));

	mD3D->OnResize();

	//--------------------------------------------------------
	// Create lights
	//--------------------------------------------------------
	mDirLights[0].Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirLights[0].Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.4f, 0.4f, 0.5f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	mDirLights[1].Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirLights[1].Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.4f, 0.4f, 0.5f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(0.57735f, 0.57735f, 0.0f);

	mDirLights[2].Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	mDirLights[2].Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(0.0f, 0.0f, -0.57735f);

	// Point lights
	mPointLights[0].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mPointLights[0].Ambient = XMFLOAT4(0.25f, 0.25, 0.25f, 0.25f);
	mPointLights[0].Specular = XMFLOAT4(0.25f, 0.25, 0.25f, 0.25f);
	mPointLights[0].Diffuse = XMFLOAT4(0.25f, 0.25, 0.25f, 0.25f);
	mPointLights[0].Attenuation = XMFLOAT3(0.25f, 0.25, 0.25f);
	mPointLights[0].Range = 50.0f;

	mPointLights[1].Position = XMFLOAT3(35.0f, 35.0f, 10.0f);
	mPointLights[1].Ambient = XMFLOAT4(0.25f, 0.25, 0.25f, 0.25f);
	mPointLights[1].Specular = XMFLOAT4(0.25f, 0.25, 0.25f, 0.25f);
	mPointLights[1].Diffuse = XMFLOAT4(0.25f, 0.25, 0.25f, 0.25f);
	mPointLights[1].Attenuation = XMFLOAT3(0.25f, 0.25, 0.25f);
	mPointLights[1].Range = 75.0f;

	mPointLights[2].Position = XMFLOAT3(-10.0f, 10.0f, -20.0f);
	mPointLights[2].Ambient = XMFLOAT4(0.25f, 0.25, 0.25f, 0.25f);
	mPointLights[2].Specular = XMFLOAT4(0.25f, 0.25, 0.25f, 0.25f);
	mPointLights[2].Diffuse = XMFLOAT4(0.25f, 0.25, 0.25f, 0.25f);
	mPointLights[2].Attenuation = XMFLOAT3(0.25f, 0.25, 0.25f);
	mPointLights[2].Range = 75.0f;

	mSky = new Sky(mD3D->GetDevice(), mTextureMgr, mResourceDir + "Textures\\SkyBox_Space.dds", 5000.0f);

	mShaderHandler = new ShaderHandler();

	// Load all the pre-compiled shaders
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\BasicVS.cso", "BasicVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\BasicPS.cso", "BasicPS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\SkyVS.cso", "SkyVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\SkyPS.cso", "SkyPS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\NormalMapSkinnedVS.cso", "NormalMapSkinnedVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\NormalMapSkinnedPS.cso", "NormalMapSkinnedPS", mD3D->GetDevice());
	//mShaderHandler->InitShaders(mD3D->GetDevice(), mInputLayouts);

	// Bind loaded shaders to shader objects
	mShaderHandler->mBasicShader->BindShaders(mShaderHandler->GetVertexShader("BasicVS"),
		mShaderHandler->GetPixelShader("BasicPS"));
	mShaderHandler->mSkyShader->BindShaders(mShaderHandler->GetVertexShader("SkyVS"),
		mShaderHandler->GetPixelShader("SkyPS"));
	mShaderHandler->mNormalSkinned->BindShaders(mShaderHandler->GetVertexShader("NormalMapSkinnedVS"),
		mShaderHandler->GetPixelShader("NormalMapSkinnedPS"));

	// Now create all the input layouts
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("BasicVS"), InputLayoutDesc::Basic32, COUNT_OF(InputLayoutDesc::Basic32), &mInputLayouts->Basic32);
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("SkyVS"), InputLayoutDesc::Position, COUNT_OF(InputLayoutDesc::Position), &mInputLayouts->Position);
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("NormalMapSkinnedVS"),
		InputLayoutDesc::PosNormalTexTanSkinned,
		COUNT_OF(InputLayoutDesc::PosNormalTexTanSkinned),
		&mInputLayouts->PosNormalTexTanSkinned);

	// Init all the shader objects
	mShaderHandler->mBasicShader->Init(mD3D->GetDevice(), mInputLayouts->Basic32);
	mShaderHandler->mSkyShader->Init(mD3D->GetDevice(), mInputLayouts->Position);
	mShaderHandler->mNormalSkinned->Init(mD3D->GetDevice(), mInputLayouts->PosNormalTexTanSkinned);

	std::string fontPath = mResourceDir + "myfile.spritefont";
	std::wstring fontPathW(fontPath.begin(), fontPath.end());

	mSpriteBatch = new SpriteBatch(mD3D->GetImmediateContext());
	mSpriteFont = new SpriteFont(mD3D->GetDevice(), fontPathW.c_str());

	return true;
}

CameraController* GraphicsEngineImpl::CreateCameraController()
{
	return CreateCameraControll(mCamera);
}

int GraphicsEngineImpl::Run()
{
	::ZeroMemory(&msg, sizeof(MSG));

	//mD3dWindow->GetTimer()->reset();

	/*while (msg.message != WM_QUIT)
	{
		DrawScene();
	}*/

	return (int)msg.wParam;
}


void GraphicsEngineImpl::DrawScene()
{
	mD3D->GetImmediateContext()->RSSetState(0);
	// Restore back and depth buffer and viewport to the OM stage
	ID3D11RenderTargetView* renderTargets[1] = { mD3D->GetRenderTargetView() };
	mD3D->GetImmediateContext()->OMSetRenderTargets(1, renderTargets, mD3D->GetDepthStencilView());
	mD3D->GetImmediateContext()->ClearRenderTargetView(mD3D->GetRenderTargetView(), reinterpret_cast<const float*>(&Colorss::LightSteelBlue));

	mD3D->GetImmediateContext()->ClearDepthStencilView(mD3D->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	mD3D->GetImmediateContext()->RSSetViewports(1, &mD3D->GetScreenViewport());

	// Draw sky
	mSky->Draw(mD3D->GetImmediateContext(), *mCamera, mShaderHandler->mSkyShader);

	// Restore default states
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	mD3D->GetImmediateContext()->RSSetState(0);
	mD3D->GetImmediateContext()->OMSetDepthStencilState(0, 0);
	mD3D->GetImmediateContext()->OMSetBlendState(0, blendFactor, 0xffffffff);
	
	// Use basic shader to draw with
	mShaderHandler->mBasicShader->SetActive(mD3D->GetImmediateContext());
	mShaderHandler->mBasicShader->SetEyePosW(mD3D->GetImmediateContext(), mCamera->GetPosition());
	mShaderHandler->mBasicShader->SetPointLights(mD3D->GetImmediateContext(), 3, mPointLights);
	mShaderHandler->mBasicShader->SetDirLights(mD3D->GetImmediateContext(), 3, mDirLights);
	mShaderHandler->mBasicShader->UpdatePerFrame(mD3D->GetImmediateContext());

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

		// Loop through all model instances
	for (UINT i = 0; i < mInstances.size(); ++i)
	{
		if (mInstances[i]->IsVisible())
		{
			//worldInvTranspose = MathHelper::InverseTranspose(mInstances[i]->GetWorld());
			//worldViewProj = mInstances[i]->GetWorld()*view*proj;

			mShaderHandler->mBasicShader->SetWorldViewProjTex(mD3D->GetImmediateContext(),
				mInstances[i]->GetWorld(),
				mCamera->GetViewProjMatrix(),
				toTexSpace);

// 			mEffects->BasicFX->SetWorld(mInstances[i]->GetWorld());
// 			mEffects->BasicFX->SetWorldInvTranspose(worldInvTranspose);
// 			mEffects->BasicFX->SetWorldViewProj(worldViewProj);
// 			mEffects->BasicFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
// 			mEffects->BasicFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

			//GenericModel* model = mInstances[i]->model;

			//for (UINT p = 0; p < techDesc.Passes; ++p)
			//{
			for (UINT j = 0; j < mInstances[i]->model->meshCount; ++j)
				{
				UINT matIndex = mInstances[i]->model->meshes[j].MaterialIndex;

// 					mEffects->BasicFX->SetMaterial(model->mat[matIndex]);
// 					mEffects->BasicFX->SetDiffuseMap(model->diffuseMapSRV[matIndex]);
				mShaderHandler->mBasicShader->SetMaterial(mD3D->GetImmediateContext(), mInstances[i]->model->mat[matIndex]);
				mShaderHandler->mBasicShader->SetDiffuseMap(mD3D->GetImmediateContext(), mInstances[i]->model->diffuseMapSRV[matIndex]);
				mShaderHandler->mBasicShader->UpdatePerObj(mD3D->GetImmediateContext());
// 
// 					activeTech->GetPassByIndex(p)->Apply(0, mD3D->GetImmediateContext());
					mD3D->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
					mInstances[i]->model->meshes[j].Draw(mD3D->GetImmediateContext());
				}
			//}
		}
	}

	/*
	// Set shader values
	mEffects->BasicFX->SetEyePosW(mCamera->GetPosition());
	mEffects->BasicFX->SetDirLights(mDirLights);

	// ---------------------------------------------------------------------
	// Specific entity draw function begin
	// ---------------------------------------------------------------------
	mD3D->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mD3D->GetImmediateContext()->IASetInputLayout(InputLayouts::Basic32);

	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	XMMATRIX view = mCamera->GetViewMatrix();
	XMMATRIX proj = mCamera->GetProjMatrix();
	XMMATRIX viewproj = mCamera->GetViewProjMatrix();

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	D3DX11_TECHNIQUE_DESC techDesc;
	ID3DX11EffectTechnique* activeTech = mEffects->BasicFX->DirLights3TexTech;
	activeTech->GetDesc(&techDesc);

	// Loop through all model instances
	for (UINT i = 0; i < mInstances.size(); ++i)
	{
		if (mInstances[i]->IsVisible())
		{
			worldInvTranspose = MathHelper::InverseTranspose(mInstances[i]->GetWorld());
			worldViewProj = mInstances[i]->GetWorld()*view*proj;

			mEffects->BasicFX->SetWorld(mInstances[i]->GetWorld());
			mEffects->BasicFX->SetWorldInvTranspose(worldInvTranspose);
			mEffects->BasicFX->SetWorldViewProj(worldViewProj);
			mEffects->BasicFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
			mEffects->BasicFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

			GenericModel* model = mInstances[i]->model;

			for (UINT p = 0; p < techDesc.Passes; ++p)
			{
				for (UINT j = 0; j < model->meshCount; ++j)
				{
					UINT matIndex = model->meshes[j].MaterialIndex;

					mEffects->BasicFX->SetMaterial(model->mat[matIndex]);
					mEffects->BasicFX->SetDiffuseMap(model->diffuseMapSRV[matIndex]);

					activeTech->GetPassByIndex(p)->Apply(0, mD3D->GetImmediateContext());
					model->meshes[j].Draw(mD3D->GetImmediateContext());
				}
			}
		}
	}

	// ---------------------------------------------------------------------
	// Specific entity draw function end
	// ---------------------------------------------------------------------
	mEffects->NormalMapFX->SetEyePosW(mCamera->GetPosition());
	mEffects->NormalMapFX->SetDirLights(mDirLights);
	*/

	mShaderHandler->mNormalSkinned->SetActive(mD3D->GetImmediateContext());
	mShaderHandler->mNormalSkinned->SetEyePosW(mD3D->GetImmediateContext(), mCamera->GetPosition());
	mShaderHandler->mNormalSkinned->SetPointLights(mD3D->GetImmediateContext(), 3, mPointLights);
	mShaderHandler->mNormalSkinned->SetDirLights(mD3D->GetImmediateContext(), 3, mDirLights);
	mShaderHandler->mNormalSkinned->UpdatePerFrame(mD3D->GetImmediateContext());

	// Draw animated character
	//mCharacter->Draw(mD3D->GetImmediateContext(), mEffects->NormalMapFX->DirLights3TexSkinnedTech, mCamera);
	//mCharacter->Draw(mD3D->GetImmediateContext(), mCamera, mShaderHandler->mNormalSkinned);

	for (UINT i = 0; i < mAnimatedInstances.size(); ++i)
	{
		if (mAnimatedInstances[i]->IsVisible())
		{
			mAnimatedInstances[i]->model->Draw(mD3D->GetImmediateContext(), mCamera, mShaderHandler->mNormalSkinned, mAnimatedInstances[i]->GetWorld());
		}
	}

	/*
	mSpriteBatch->Begin();
	mSpriteBatch->Draw(mTextureMgr->GetTexture(L"Data\\Models\\WoodBlock.dds"), XMFLOAT2(300.0f, 400.0f));
	mSpriteFont->DrawString(mSpriteBatch, L"Test", XMFLOAT2(100.0f, 100.0f), Colorss::Green, 0.0f, XMFLOAT2(100.0f, 100.0f), XMFLOAT2(1.0f, 1.0f));
	mSpriteBatch->End();
	*/
	

	// Restore default states
	mD3D->GetImmediateContext()->RSSetState(0);
	mD3D->GetImmediateContext()->OMSetDepthStencilState(0, 0);
	mD3D->GetImmediateContext()->OMSetBlendState(0, blendFactor, 0xffffffff);

	HR(mD3D->GetSwapChain()->Present(0, 0));
}

void GraphicsEngineImpl::UpdateScene(float dt)
{
	//std::cout << "Updating scene!\n";

	//mCamera->UpdateViewMatrix();


	for (size_t i = 0; i < mAnimatedInstances.size(); i++)
	{
		mAnimatedInstances[i]->model->SetKeyFrameInterval(mAnimatedInstances[i]->model->mAnimations[mAnimatedInstances[i]->model->mCurAnim].FrameStart, mAnimatedInstances[i]->model->mAnimations[mAnimatedInstances[i]->model->mCurAnim].FrameEnd);
		mAnimatedInstances[i]->model->Update(dt);
	}
}
ModelInstance* GraphicsEngineImpl::CreateInstance(std::string file)
{
	return CreateInstance(file, Vec3());
}
ModelInstance* GraphicsEngineImpl::CreateInstance(std::string file, Vec3 pos)
{
	if (mModels.find(file) == mModels.end())
	{
		std::stringstream ss;
		ss << file << ".obj";

		mModels[file] = new GenericModel(mD3D->GetDevice(),
			mTextureMgr,
			mResourceDir + ss.str(),
			mResourceDir + "Models\\");
	}

	ModelInstanceImpl* mi = new ModelInstanceImpl(pos, Vec3(0.0f, 0.0f, 0.0f), Vec3(1, 1, 1));
	mi->model = mModels[file];

	mInstances.push_back(mi);
	return mi;
}

void GraphicsEngineImpl::DeleteInstance(ModelInstance* m)
{
	ModelInstanceImpl* mi = (ModelInstanceImpl*)m;

	bool found = false;
	int index = -1;
	for (unsigned int i = 0; i < mInstances.size(); i++)
	{
		if (mi == mInstances[i])
			index = i;
		else if (mi->model == mInstances[i]->model)
			found = true;
	}

	if (index != -1)
		mInstances.erase(mInstances.begin() + index);

	if (!found) //delete model if no other instance uses it
	{
		for (std::map<std::string, GenericModel*>::iterator it = mModels.begin(); it != mModels.end(); it++)
		{
			if (it->second == mi->model)
			{
				mModels.erase(it);
				break;
			}
		}
		delete mi->model;
	}

	delete mi;
}


AnimatedInstance* GraphicsEngineImpl::CreateAnimatedInstance(std::string file)
{
	if (mModels.find(file) == mModels.end())
	{
		std::stringstream ss;
		ss << file << ".dae";
		mSkinnedModels[file] = new GenericSkinnedModel(mD3D->GetDevice(),
			mTextureMgr,
			mResourceDir + ss.str(),
			mResourceDir + "Models\\Character\\");
	}

	AnimatedInstanceImpl* mi = new AnimatedInstanceImpl(Vec3(), Vec3(), Vec3(1, 1, 1));
	mi->model = new AnimatedEntity(mSkinnedModels[file], XMFLOAT3(0,0,0));

	

	mAnimatedInstances.push_back(mi);
	return mi;
}
void GraphicsEngineImpl::DeleteInstance(AnimatedInstance* ai)
{
	AnimatedInstanceImpl* mi = (AnimatedInstanceImpl*)ai;

	bool found = false;
	int index = -1;
	for (unsigned int i = 0; i < mAnimatedInstances.size(); i++)
	{
		if (mi == mAnimatedInstances[i])
			index = i;
		else if (mi->model->mInstance.model == mAnimatedInstances[i]->model->mInstance.model)
			found = true;
	}

	if (index != -1)
		mInstances.erase(mInstances.begin() + index);

	if (!found) //delete model if no other instance uses it
	{
		for (std::map<std::string, GenericSkinnedModel*>::iterator it = mSkinnedModels.begin(); it != mSkinnedModels.end(); it++)
		{
			if (it->second == mi->model->mInstance.model)
			{
				mSkinnedModels.erase(it);
				break;
			}
		}
		delete mi->model;
	}

	delete mi;
}
