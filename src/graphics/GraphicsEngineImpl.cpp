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

	delete mSky;
	delete mShadowMap;
	delete mCamera;
	delete mTextureMgr;

	mInputLayouts->DestroyAll();
	delete mInputLayouts;
	RenderStates::DestroyAll();

	delete mSpriteFont;
	delete mSpriteBatch;

	delete mShaderHandler;

	delete mD3D;
}

bool GraphicsEngineImpl::Init(HWND hWindow, int width, int height, const std::string &resourceDir)
{
	mResourceDir = resourceDir;

	mD3D = new Direct3D();
	mD3D->Init(&hWindow, width, height);

	mInputLayouts = new InputLayouts();

	RenderStates::InitAll(mD3D->GetDevice());

	// Texture manager
	mTextureMgr = new TextureManager();
	mTextureMgr->Init(mD3D->GetDevice(), mD3D->GetImmediateContext());

	// Camera
	mCamera = new Camera();
	mCamera->SetLens(0.25f*MathHelper::pi, static_cast<float>(width) / height, 1.0f, 10000.0f);
	mCamera->SetPosition(XMFLOAT3(0.0f, 0.0f, -400.0f));

	//--------------------------------------------------------
	// Lights
	//--------------------------------------------------------
	DirectionalLight dirLight;
	ZeroMemory(&dirLight, sizeof(DirectionalLight));

	dirLight.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	dirLight.Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	dirLight.Specular = XMFLOAT4(0.4f, 0.4f, 0.5f, 1.0f);
	dirLight.Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	dirLight.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	dirLight.Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	dirLight.Specular = XMFLOAT4(0.4f, 0.4f, 0.5f, 1.0f);
	dirLight.Direction = XMFLOAT3(0.00f, -1.0f, 0.00f);

	mDirLights.push_back(dirLight);

	dirLight.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	dirLight.Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	dirLight.Specular = XMFLOAT4(0.4f, 0.4f, 0.5f, 1.0f);
	dirLight.Direction = XMFLOAT3(0.57735f, 0.57735f, 0.0f);
	mDirLights.push_back(dirLight);

	dirLight.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	dirLight.Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	dirLight.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	dirLight.Direction = XMFLOAT3(0.0f, 0.0f, -0.57735f);
	mDirLights.push_back(dirLight);

	// Point lights
	PointLight pointLight;
	ZeroMemory(&pointLight, sizeof(PointLight));

	pointLight.Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	pointLight.Ambient = XMFLOAT4(0.25f, 0.25, 0.25f, 0.25f);
	pointLight.Specular = XMFLOAT4(0.25f, 0.25, 0.25f, 0.25f);
	pointLight.Diffuse = XMFLOAT4(0.25f, 0.25, 0.25f, 0.25f);
	pointLight.Attenuation = XMFLOAT3(0.25f, 0.25, 0.25f);
	pointLight.Range = 50.0f;
	mPointLights.push_back(pointLight);

	pointLight.Position = XMFLOAT3(35.0f, 35.0f, 10.0f);
	pointLight.Ambient = XMFLOAT4(0.25f, 0.25, 0.25f, 0.25f);
	pointLight.Specular = XMFLOAT4(0.25f, 0.25, 0.25f, 0.25f);
	pointLight.Diffuse = XMFLOAT4(0.25f, 0.25, 0.25f, 0.25f);
	pointLight.Attenuation = XMFLOAT3(0.25f, 0.25, 0.25f);
	pointLight.Range = 75.0f;
	mPointLights.push_back(pointLight);

	pointLight.Position = XMFLOAT3(-10.0f, 10.0f, -20.0f);
	pointLight.Ambient = XMFLOAT4(0.25f, 0.25, 0.25f, 0.25f);
	pointLight.Specular = XMFLOAT4(0.25f, 0.25, 0.25f, 0.25f);
	pointLight.Diffuse = XMFLOAT4(0.25f, 0.25, 0.25f, 0.25f);
	pointLight.Attenuation = XMFLOAT3(0.25f, 0.25, 0.25f);
	pointLight.Range = 75.0f;
	mPointLights.push_back(pointLight);

	mSky = new Sky(mD3D->GetDevice(), mTextureMgr, mResourceDir + "Textures\\SkyBox_Space.dds", 5000.0f);
	mShadowMap = new ShadowMap(mD3D->GetDevice(), 2048, 2048);


	//-------------------------------------------------------------------------------------------------------
	// Shaders
	//-------------------------------------------------------------------------------------------------------
	mShaderHandler = new ShaderHandler();

	// Load all the pre-compiled shaders
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\BasicVS.cso", "BasicVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\BasicPS.cso", "BasicPS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\SkyVS.cso", "SkyVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\SkyPS.cso", "SkyPS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\NormalMapSkinnedVS.cso", "NormalMapSkinnedVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\NormalMapSkinnedPS.cso", "NormalMapSkinnedPS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\ShadowBuildVS.cso", "ShadowBuildVS", mD3D->GetDevice());

	// Bind loaded shaders to shader objects
	mShaderHandler->mBasicShader->BindShaders(mShaderHandler->GetVertexShader("BasicVS"),
		mShaderHandler->GetPixelShader("BasicPS"));
	mShaderHandler->mSkyShader->BindShaders(mShaderHandler->GetVertexShader("SkyVS"),
		mShaderHandler->GetPixelShader("SkyPS"));
	mShaderHandler->mNormalSkinned->BindShaders(mShaderHandler->GetVertexShader("NormalMapSkinnedVS"),
		mShaderHandler->GetPixelShader("NormalMapSkinnedPS"));
	//mShaderHandler->mShadowShader->BindShaders(mShaderHandler->GetVertexShader("ShadowBuildVS"), mShaderHandler->GetPixelShader(""));
	mShaderHandler->mShadowShader->BindVertexShader(mShaderHandler->GetVertexShader("ShadowBuildVS"));

	// Now create all the input layouts
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("BasicVS"), InputLayoutDesc::Basic32, COUNT_OF(InputLayoutDesc::Basic32), &mInputLayouts->Basic32);
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("ShadowBuildVS"), InputLayoutDesc::Position, COUNT_OF(InputLayoutDesc::Position), &mInputLayouts->Position);
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("SkyVS"), InputLayoutDesc::Position, COUNT_OF(InputLayoutDesc::Position), &mInputLayouts->Position);
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("NormalMapSkinnedVS"),
		InputLayoutDesc::PosNormalTexTanSkinned,
		COUNT_OF(InputLayoutDesc::PosNormalTexTanSkinned),
		&mInputLayouts->PosNormalTexTanSkinned);

	// Init all the shader objects
	mShaderHandler->mBasicShader->Init(mD3D->GetDevice(), mInputLayouts->Basic32);
	mShaderHandler->mSkyShader->Init(mD3D->GetDevice(), mInputLayouts->Position);
	mShaderHandler->mNormalSkinned->Init(mD3D->GetDevice(), mInputLayouts->PosNormalTexTanSkinned);
	mShaderHandler->mShadowShader->Init(mD3D->GetDevice(), mInputLayouts->Position);

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

void GraphicsEngineImpl::Run(float dt)
{
	UpdateScene(dt);
	DrawScene();
}

void GraphicsEngineImpl::DrawScene()
{
	// Draw scene to shadowmap
	mShadowMap->BindDsvAndSetNullRenderTarget(mD3D->GetImmediateContext());
	mShadowMap->BuildShadowTransform(mDirLights.at(0), XMFLOAT3(0.0f, 0.0f, 0.0f), 100.0f);
	mShaderHandler->mShadowShader->SetActive(mD3D->GetImmediateContext());
	mShadowMap->DrawSceneToShadowMap(mInstances, mAnimatedInstances, *mCamera, mD3D->GetImmediateContext(), mShaderHandler->mShadowShader);
	

	mD3D->GetImmediateContext()->RSSetState(0);
	// Restore back and depth buffer and viewport to the OM stage
	ID3D11RenderTargetView* renderTargets[1] = { mD3D->GetRenderTargetView() };
	mD3D->GetImmediateContext()->OMSetRenderTargets(1, renderTargets, mD3D->GetDepthStencilView());
	mD3D->GetImmediateContext()->ClearRenderTargetView(mD3D->GetRenderTargetView(), reinterpret_cast<const float*>(&D3dColors::LightSteelBlue));

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
	mShaderHandler->mBasicShader->SetPointLights(mD3D->GetImmediateContext(), mPointLights.size(), mPointLights.data());
	mShaderHandler->mBasicShader->SetDirLights(mD3D->GetImmediateContext(), mDirLights.size(), mDirLights.data());
	mShaderHandler->mBasicShader->SetShadowMap(mD3D->GetImmediateContext(), mShadowMap->getDepthMapSRV());
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
			mShaderHandler->mBasicShader->SetWorldViewProjTex(mD3D->GetImmediateContext(),
				mInstances[i]->GetWorld(),
				mCamera->GetViewProjMatrix()/*mShadowMap->GetLightViewProj()*/,
				toTexSpace);

			XMMATRIX test = XMMatrixMultiply(mInstances[i]->GetWorld(), mShadowMap->GetLightViewProj());
			XMFLOAT4X4 test1;
			XMStoreFloat4x4(&test1, test);
			mShaderHandler->mBasicShader->SetShadowTransform(mD3D->GetImmediateContext(), /*test1*/mShadowMap->GetShadowTransform());
			for (UINT j = 0; j < mInstances[i]->model->meshCount; ++j)
			{
				UINT matIndex = mInstances[i]->model->meshes[j].MaterialIndex;

				mShaderHandler->mBasicShader->SetMaterial(mD3D->GetImmediateContext(), mInstances[i]->model->mat[matIndex]);
				mShaderHandler->mBasicShader->SetDiffuseMap(mD3D->GetImmediateContext(), mInstances[i]->model->diffuseMapSRV[matIndex]/*mShadowMap->getDepthMapSRV()*/);
				mShaderHandler->mBasicShader->UpdatePerObj(mD3D->GetImmediateContext());
				mD3D->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				mInstances[i]->model->meshes[j].Draw(mD3D->GetImmediateContext());
			}
		}
	}

	// Use normal mapping skinned shader to draw with
	mShaderHandler->mNormalSkinned->SetActive(mD3D->GetImmediateContext());
	mShaderHandler->mNormalSkinned->SetEyePosW(mD3D->GetImmediateContext(), mCamera->GetPosition());
	mShaderHandler->mNormalSkinned->SetPointLights(mD3D->GetImmediateContext(), mPointLights.size(), mPointLights.data());
	mShaderHandler->mNormalSkinned->SetDirLights(mD3D->GetImmediateContext(), mDirLights.size(), mDirLights.data());
	mShaderHandler->mNormalSkinned->UpdatePerFrame(mD3D->GetImmediateContext());

	// Loop through all skinned model instances
	for (UINT i = 0; i < mAnimatedInstances.size(); ++i)
	{
		if (mAnimatedInstances[i]->IsVisible())
		{
			mAnimatedInstances[i]->model->Draw(mD3D->GetImmediateContext(), mCamera, mShaderHandler->mNormalSkinned, mAnimatedInstances[i]->GetWorld());
		}
	}

	// Draw 2D stuff
	/*
	mSpriteBatch->Begin();
	mSpriteBatch->Draw(mTextureMgr->CreateTexture("..\\..\\content\\Models\\WoodBlock.dds"), XMFLOAT2(300.0f, 400.0f));
	mSpriteFont->DrawString(mSpriteBatch, L"Test", XMFLOAT2(100.0f, 100.0f), Colorss::Green, 0.0f, XMFLOAT2(100.0f, 100.0f), XMFLOAT2(1.0f, 1.0f));
	mSpriteBatch->End();
	*/

	//ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	//mD3D->GetImmediateContext()->PSSetShaderResources(0, 16, nullSRV);

	// Restore default states
	mD3D->GetImmediateContext()->RSSetState(0);
	mD3D->GetImmediateContext()->OMSetDepthStencilState(0, 0);
	mD3D->GetImmediateContext()->OMSetBlendState(0, blendFactor, 0xffffffff);

	// Finally, present the back buffer to front buffer
	// Set SyncInterval to 1 if you want to limit the FPS to the monitors refresh rate
	HR(mD3D->GetSwapChain()->Present(1, 0));
}

void GraphicsEngineImpl::UpdateScene(float dt)
{
	// Update skinned instances
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
	mi->model = new AnimatedEntity(mSkinnedModels[file], XMFLOAT3(0, 0, 0));

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
