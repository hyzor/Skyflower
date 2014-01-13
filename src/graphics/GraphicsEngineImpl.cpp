#include "GraphicsEngineImpl.h"
#include "Texture2DImpl.h"

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
	delete mDeferredBuffers;

	mOrthoWindow->Shutdown();
	delete mOrthoWindow;

	delete mD3D;
}

bool GraphicsEngineImpl::Init(HWND hWindow, UINT width, UINT height, const std::string &resourceDir)
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
	mCamera->SetLens(0.25f*MathHelper::pi, static_cast<float>(width) / height, zNear, zFar);
	mCamera->UpdateOrthoMatrix(static_cast<float>(width), static_cast<float>(height), zNear, zFar);
	mCamera->SetPosition(XMFLOAT3(0.0f, 0.0f, -400.0f));
	mCamera->UpdateBaseViewMatrix();
	mCamera->Update();

	mDeferredBuffers = new DeferredBuffers();
	mDeferredBuffers->Init(mD3D->GetDevice(), width, height, zNear, zFar);

	//--------------------------------------------------------
	// Lights
	//--------------------------------------------------------
	// Directional lights
	DirectionalLight dirLight;
	ZeroMemory(&dirLight, sizeof(DirectionalLight));

	dirLight.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	dirLight.Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	dirLight.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	dirLight.Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	dirLight.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	dirLight.Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	dirLight.Specular = XMFLOAT4(0.4f, 0.4f, 0.5f, 1.0f);
	dirLight.Direction = XMFLOAT3(0.1f, -1.0f, 0.1f);

	mDirLights.push_back(dirLight);

	//dirLight.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	//dirLight.Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	//dirLight.Specular = XMFLOAT4(0.4f, 0.4f, 0.5f, 1.0f);
	//dirLight.Direction = XMFLOAT3(0.57735f, 0.57735f, 0.0f);
	//mDirLights.push_back(dirLight);

	// Point lights
	PointLight pointLight;
	ZeroMemory(&pointLight, sizeof(PointLight));

	pointLight.Position = XMFLOAT3(-20.0f, 20.0f, 0.0f);
	pointLight.Ambient = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	pointLight.Specular = XMFLOAT4(0.40f, 0.40f, 0.40f, 1.0f);
	pointLight.Diffuse = XMFLOAT4(0.40f, 0.40f, 0.40f, 1.0f);
	pointLight.Attenuation = XMFLOAT3(0.1f, 0.1f, 0.0f);
	pointLight.Range = 100.0f;
	//mPointLights.push_back(pointLight);

// 	pointLight.Position = XMFLOAT3(35.0f, 35.0f, 10.0f);
// 	pointLight.Ambient = XMFLOAT4(0.25f, 0.25f, 0.25f, 0.25f);
// 	pointLight.Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 0.25f);
// 	pointLight.Diffuse = XMFLOAT4(0.25f, 0.25f, 0.25f, 0.25f);
// 	pointLight.Attenuation = XMFLOAT3(0.25f, 0.25f, 0.25f);
// 	pointLight.Range = 75.0f;
// 	mPointLights.push_back(pointLight);
// 
// 	pointLight.Position = XMFLOAT3(-10.0f, 10.0f, -20.0f);
// 	pointLight.Ambient = XMFLOAT4(0.25f, 0.25f, 0.25f, 0.25f);
// 	pointLight.Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 0.25f);
// 	pointLight.Diffuse = XMFLOAT4(0.25f, 0.25f, 0.25f, 0.25f);
// 	pointLight.Attenuation = XMFLOAT3(0.25f, 0.25, 0.25f);
// 	pointLight.Range = 75.0f;
// 	mPointLights.push_back(pointLight);

	// Spot lights
	SpotLight spotLight;
	ZeroMemory(&spotLight, sizeof(SpotLight));

	spotLight.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	spotLight.Diffuse = XMFLOAT4(0.65f, 0.65f, 0.65f, 1.0f);
	spotLight.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	spotLight.Attenuation = XMFLOAT3(1.0f, 0.0f, 0.0f);
	spotLight.Spot = 45.0f;
	spotLight.Range = 1000.0f;
	spotLight.Position = XMFLOAT3(0.0f, 200.0f, 0.0f);
	spotLight.Direction = XMFLOAT3(0.1f, -1.0f, 0.1f);
	mSpotLights.push_back(spotLight);

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
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\ShadowBuildPS.cso", "ShadowBuildPS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\SkinnedShadowBuildVS.cso", "SkinnedShadowBuildVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\SkinnedShadowBuildPS.cso", "SkinnedShadowBuildPS", mD3D->GetDevice());

	// Deferred shaders
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\BasicDeferredVS.cso", "BasicDeferredVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\BasicDeferredPS.cso", "BasicDeferredPS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\LightDeferredVS.cso", "LightDeferredVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\LightDeferredPS.cso", "LightDeferredPS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\BasicDeferredSkinnedVS.cso", "BasicDeferredSkinnedVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\BasicDeferredSkinnedPS.cso", "BasicDeferredSkinnedPS", mD3D->GetDevice());

	// Bind loaded shaders to shader objects
	mShaderHandler->mBasicShader->BindShaders(
		mShaderHandler->GetVertexShader("BasicVS"),
		mShaderHandler->GetPixelShader("BasicPS"));
	mShaderHandler->mSkyShader->BindShaders(
		mShaderHandler->GetVertexShader("SkyVS"),
		mShaderHandler->GetPixelShader("SkyPS"));
	mShaderHandler->mNormalSkinned->BindShaders(
		mShaderHandler->GetVertexShader("NormalMapSkinnedVS"),
		mShaderHandler->GetPixelShader("NormalMapSkinnedPS"));
	mShaderHandler->mBasicDeferredShader->BindShaders(
		mShaderHandler->GetVertexShader("BasicDeferredVS"),
		mShaderHandler->GetPixelShader("BasicDeferredPS"));
	mShaderHandler->mSkinnedShadowShader->BindShaders(mShaderHandler->GetVertexShader("SkinnedShadowBuildVS"),
		mShaderHandler->GetPixelShader("SkinnedShadowBuildPS"));
	mShaderHandler->mBasicDeferredSkinnedShader->BindShaders(
		mShaderHandler->GetVertexShader("BasicDeferredSkinnedVS"),
		mShaderHandler->GetPixelShader("BasicDeferredSkinnedPS"));
	mShaderHandler->mLightDeferredShader->BindShaders(
		mShaderHandler->GetVertexShader("LightDeferredVS"),
		mShaderHandler->GetPixelShader("LightDeferredPS"));
	//mShaderHandler->mShadowShader->BindShaders(mShaderHandler->GetVertexShader("ShadowBuildVS"), mShaderHandler->GetPixelShader(""));
	//mShaderHandler->mShadowShader->BindVertexShader(mShaderHandler->GetVertexShader("ShadowBuildVS"));
	mShaderHandler->mShadowShader->BindShaders(mShaderHandler->GetVertexShader("ShadowBuildVS"), mShaderHandler->GetPixelShader("ShadowBuildPS"));

	// Now create all the input layouts
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("BasicVS"), InputLayoutDesc::PosNormalTex, COUNT_OF(InputLayoutDesc::PosNormalTex), &mInputLayouts->PosNormalTex);
	//mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("ShadowBuildVS"), InputLayoutDesc::Position, COUNT_OF(InputLayoutDesc::Position), &mInputLayouts->Position);
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("SkyVS"), InputLayoutDesc::Position, COUNT_OF(InputLayoutDesc::Position), &mInputLayouts->Position);
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("NormalMapSkinnedVS"),
		InputLayoutDesc::PosNormalTexTanSkinned,
		COUNT_OF(InputLayoutDesc::PosNormalTexTanSkinned),
		&mInputLayouts->PosNormalTexTanSkinned);
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("LightDeferredVS"), InputLayoutDesc::PosTex, COUNT_OF(InputLayoutDesc::PosTex), &mInputLayouts->PosTex);

	// Init all the shader objects
	mShaderHandler->mBasicShader->Init(mD3D->GetDevice(), mInputLayouts->PosNormalTex);
	mShaderHandler->mSkyShader->Init(mD3D->GetDevice(), mInputLayouts->Position);
	mShaderHandler->mNormalSkinned->Init(mD3D->GetDevice(), mInputLayouts->PosNormalTexTanSkinned);
	mShaderHandler->mBasicDeferredShader->Init(mD3D->GetDevice(), mInputLayouts->PosNormalTex);
	mShaderHandler->mBasicDeferredSkinnedShader->Init(mD3D->GetDevice(), mInputLayouts->PosNormalTexTanSkinned);
	mShaderHandler->mLightDeferredShader->Init(mD3D->GetDevice(), mInputLayouts->PosTex);
	mShaderHandler->mShadowShader->Init(mD3D->GetDevice(), mInputLayouts->PosNormalTex);
	mShaderHandler->mSkinnedShadowShader->Init(mD3D->GetDevice(), mInputLayouts->PosNormalTexTanSkinned);

	std::string fontPath = mResourceDir + "myfile.spritefont";
	std::wstring fontPathW(fontPath.begin(), fontPath.end());

	mSpriteBatch = new SpriteBatch(mD3D->GetImmediateContext());
	mSpriteFont = new SpriteFont(mD3D->GetDevice(), fontPathW.c_str());

	// Create orthogonal window
	mOrthoWindow = new OrthoWindow();
	mOrthoWindow->Initialize(mD3D->GetDevice(), width, height);

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
	ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	mD3D->GetImmediateContext()->PSSetShaderResources(0, 16, nullSRV);
	mD3D->GetImmediateContext()->VSSetShaderResources(0, 16, nullSRV);

	// Draw scene to shadowmap
	mD3D->GetImmediateContext()->RSSetState(RenderStates::mDepthBiasRS); // This rasterizer state fixes shadow acne
	mShadowMap->BindDsvAndSetNullRenderTarget(mD3D->GetImmediateContext());
	mShadowMap->BuildShadowTransform(mDirLights.at(0), mSceneBounds);
	mShadowMap->DrawSceneToShadowMap(mInstances, mAnimatedInstances, mD3D->GetImmediateContext(), mShaderHandler->mShadowShader, mShaderHandler->mSkinnedShadowShader);

	mD3D->GetImmediateContext()->RSSetState(0);
	// Restore back and depth buffer and viewport to the OM stage
	ID3D11RenderTargetView* renderTargets[1] = { mD3D->GetRenderTargetView() };
	mD3D->GetImmediateContext()->OMSetRenderTargets(1, renderTargets, mD3D->GetDepthStencilView());
	mD3D->GetImmediateContext()->ClearRenderTargetView(mD3D->GetRenderTargetView(), reinterpret_cast<const float*>(&D3dColors::LightSteelBlue));

	mD3D->GetImmediateContext()->ClearDepthStencilView(mD3D->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	mD3D->GetImmediateContext()->RSSetViewports(1, &mD3D->GetScreenViewport());

	// Draw sky
	//mSky->Draw(mD3D->GetImmediateContext(), *mCamera, mShaderHandler->mSkyShader);

	// Restore default states
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	mD3D->GetImmediateContext()->RSSetState(0);
	mD3D->GetImmediateContext()->OMSetDepthStencilState(0, 0);
	mD3D->GetImmediateContext()->OMSetBlendState(0, blendFactor, 0xffffffff);

	// Draw 2D stuff
	/*
	mSpriteBatch->Begin();
	mSpriteBatch->Draw(mTextureMgr->CreateTexture("..\\..\\content\\Models\\WoodBlock.dds"), XMFLOAT2(300.0f, 400.0f));
	mSpriteFont->DrawString(mSpriteBatch, L"Test", XMFLOAT2(100.0f, 100.0f), D3dColors::Green, 0.0f, XMFLOAT2(100.0f, 100.0f), XMFLOAT2(1.0f, 1.0f));
	mSpriteBatch->End();
	*/

	//------------------------------------------------------------------------------
	// Deferred shading
	//------------------------------------------------------------------------------
	// First clear the textures (otherwise D3D11 WARNING)
	mShaderHandler->mLightDeferredShader->SetDiffuseTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mLightDeferredShader->SetNormalTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mLightDeferredShader->SetSpecularTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mLightDeferredShader->SetPositionTexture(mD3D->GetImmediateContext(), NULL);

	// Render the scene to the render buffers
	RenderSceneToTexture();

	// Turn off Z-buffer to begin 2D-drawing
	mD3D->GetImmediateContext()->OMSetDepthStencilState(RenderStates::mDisabledDSS, 1);
	mShaderHandler->mLightDeferredShader->SetActive(mD3D->GetImmediateContext());
	mShaderHandler->mLightDeferredShader->SetEyePosW(mCamera->GetPosition());
	mShaderHandler->mLightDeferredShader->SetPointLights(mD3D->GetImmediateContext(), (UINT)mPointLights.size(), mPointLights.data());
	mShaderHandler->mLightDeferredShader->SetDirLights(mD3D->GetImmediateContext(), (UINT)mDirLights.size(), mDirLights.data());
	mShaderHandler->mLightDeferredShader->SetSpotLights(mD3D->GetImmediateContext(), (UINT)mSpotLights.size(), mSpotLights.data());
	mShaderHandler->mLightDeferredShader->SetShadowMapTexture(mD3D->GetImmediateContext(), mShadowMap->getDepthMapSRV());

	//XMMATRIX cameraViewProj = mCamera->GetViewMatrix()*mCamera->GetProjMatrix();
	//mShaderHandler->mLightDeferredShader->SetShadowTransform(cameraViewProj);
	mShaderHandler->mLightDeferredShader->SetShadowTransform(mShadowMap->GetShadowTransform());
	mShaderHandler->mLightDeferredShader->SetCameraViewMatrix(mCamera->GetViewMatrix());
	mShaderHandler->mLightDeferredShader->SetLightWorldViewProj(mShadowMap->GetLightWorld(), mShadowMap->GetLightView(), mShadowMap->GetLightProj());
	mShaderHandler->mLightDeferredShader->UpdatePerFrame(mD3D->GetImmediateContext());

	mShaderHandler->mLightDeferredShader->SetDiffuseTexture(mD3D->GetImmediateContext(), mDeferredBuffers->GetSRV(0));
	mShaderHandler->mLightDeferredShader->SetNormalTexture(mD3D->GetImmediateContext(), mDeferredBuffers->GetSRV(1));
	mShaderHandler->mLightDeferredShader->SetSpecularTexture(mD3D->GetImmediateContext(), mDeferredBuffers->GetSRV(2));
	mShaderHandler->mLightDeferredShader->SetPositionTexture(mD3D->GetImmediateContext(), mDeferredBuffers->GetSRV(3));

	mShaderHandler->mLightDeferredShader->SetWorldViewProj(XMMatrixIdentity(), mCamera->GetBaseViewMatrix(), mCamera->GetOrthoMatrix());
	mShaderHandler->mLightDeferredShader->UpdatePerObj(mD3D->GetImmediateContext());

	// Now render the window
	mOrthoWindow->Render(mD3D->GetImmediateContext());

	// Turn z-buffer back on
	mD3D->GetImmediateContext()->OMSetDepthStencilState(RenderStates::mDefaultDSS, 1);

	mSpriteBatch->Begin();
	mSpriteBatch->Draw(mDeferredBuffers->GetSRV(0), XMFLOAT2(0.0f, 0.0f), NULL, Colors::White, 0.0f, XMFLOAT2(0.0f, 0.0f), 0.2f);
	mSpriteBatch->Draw(mDeferredBuffers->GetSRV(1), XMFLOAT2(0.0f, 150.0f), NULL, Colors::White, 0.0f, XMFLOAT2(0.0f, 0.0f), 0.2f);
	mSpriteBatch->Draw(mDeferredBuffers->GetSRV(2), XMFLOAT2(0.0f, 300.0f), NULL, Colors::White, 0.0f, XMFLOAT2(0.0f, 0.0f), 0.2f);
	mSpriteBatch->Draw(mDeferredBuffers->GetSRV(3), XMFLOAT2(0.0f, 450.0f), NULL, Colors::White, 0.0f, XMFLOAT2(0.0f, 0.0f), 0.2f);

	//mD3D->GetImmediateContext()->OMSetDepthStencilState(RenderStates::)
	
	//mSpriteBatch->Draw(mDeferredBuffers->GetSRV(3), XMFLOAT2(0.0f, 200.0f), NULL, Colors::White, 0.0f, XMFLOAT2(0.0f, 0.0f), 0.25f);
	//mSpriteFont->DrawString(mSpriteBatch, L"Test", XMFLOAT2(100.0f, 100.0f), D3dColors::Green, 0.0f, XMFLOAT2(100.0f, 100.0f), XMFLOAT2(1.0f, 1.0f));
	mSpriteBatch->End();

	mSpriteBatch->Begin(SpriteSortMode_Deferred, nullptr, nullptr, RenderStates::mDisabledDSS, nullptr);
	mSpriteBatch->Draw(mShadowMap->getDepthMapSRV(), XMFLOAT2(0.0f, 600.0f), NULL, Colors::White, 0.0f, XMFLOAT2(0.0f, 0.0f), 0.1f);
	mSpriteBatch->End();

	//mSky->Draw(mD3D->GetImmediateContext(), *mCamera, mShaderHandler->mSkyShader);
}

void GraphicsEngineImpl::UpdateScene(float dt)
{
	// Update skinned instances
	for (size_t i = 0; i < mAnimatedInstances.size(); i++)
	{
		mAnimatedInstances[i]->model->SetKeyFrameInterval(mAnimatedInstances[i]->model->mAnimations[mAnimatedInstances[i]->model->mCurAnim].FrameStart, mAnimatedInstances[i]->model->mAnimations[mAnimatedInstances[i]->model->mCurAnim].FrameEnd);
		mAnimatedInstances[i]->model->Update(dt);
	}

	mCamera->Update();
}

void GraphicsEngineImpl::Present()
{
	// Present the back buffer to front buffer
	// Set SyncInterval to 1 if you want to limit the FPS to the monitors refresh rate
	HR(mD3D->GetSwapChain()->Present(1, 0));
}

void GraphicsEngineImpl::Begin2D()
{
	mSpriteBatch->Begin();
}

void GraphicsEngineImpl::End2D()
{
	mSpriteBatch->End();
}

void GraphicsEngineImpl::Draw2DTextureFile(const std::string file, int x, int y)
{
	mSpriteBatch->Draw(mTextureMgr->CreateTexture(file), XMFLOAT2((float)x, (float)y));
}

void GraphicsEngineImpl::Draw2DTexture(Texture2D *texture, int x, int y)
{
	Texture2DImpl *textureImpl = (Texture2DImpl *)texture;

	mSpriteBatch->Draw(textureImpl->GetTextureView(), XMFLOAT2((float)x, (float)y));
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

Texture2D *GraphicsEngineImpl::CreateTexture2D(unsigned int width, unsigned int height)
{
	Texture2DImpl *texture = new Texture2DImpl(mD3D->GetDevice(), mD3D->GetImmediateContext(), width, height);
 
	return (Texture2D *)texture;
}

void GraphicsEngineImpl::DeleteTexture2D(Texture2D *texture)
{
	Texture2DImpl *textureImpl = (Texture2DImpl *)texture;

	delete textureImpl;
}

void GraphicsEngineImpl::OnResize(UINT width, UINT height)
{
	mD3D->OnResize(width, height);
	mDeferredBuffers->OnResize(width, height);
	mCamera->UpdateOrthoMatrix(static_cast<float>(width), static_cast<float>(height), zNear, zFar);
	// TODO:
	//mOrthoWindow->OnResize(width, height);
}

void GraphicsEngineImpl::RenderSceneToTexture()
{
	mDeferredBuffers->SetRenderTargets(mD3D->GetImmediateContext(), mD3D->GetDepthStencilView());
	mD3D->GetImmediateContext()->RSSetViewports(1, &mD3D->GetScreenViewport());

	mDeferredBuffers->ClearRenderTargets(mD3D->GetImmediateContext(), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), mD3D->GetDepthStencilView());

	mShaderHandler->mBasicDeferredShader->SetActive(mD3D->GetImmediateContext());

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
			mShaderHandler->mBasicDeferredShader->SetWorldViewProjTex(mInstances[i]->GetWorld(),
				mCamera->GetViewProjMatrix(),
				toTexSpace);

			for (UINT j = 0; j < mInstances[i]->model->meshCount; ++j)
			{
				UINT matIndex = mInstances[i]->model->meshes[j].MaterialIndex;

				mShaderHandler->mBasicDeferredShader->SetMaterial(mInstances[i]->model->mat[matIndex]);
				mShaderHandler->mBasicDeferredShader->SetDiffuseMap(mD3D->GetImmediateContext(), mInstances[i]->model->diffuseMapSRV[matIndex]);
				mShaderHandler->mBasicDeferredShader->UpdatePerObj(mD3D->GetImmediateContext());
				mD3D->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				mInstances[i]->model->meshes[j].Draw(mD3D->GetImmediateContext());
			}
		}
	}

	mShaderHandler->mBasicDeferredSkinnedShader->SetActive(mD3D->GetImmediateContext());

	// Loop through all model instances
	for (UINT i = 0; i < mAnimatedInstances.size(); ++i)
	{
		if (mAnimatedInstances[i]->IsVisible())
		{
			mAnimatedInstances[i]->model->Draw(mD3D->GetImmediateContext(), mCamera, mShaderHandler->mBasicDeferredSkinnedShader, mAnimatedInstances[i]->GetWorld());
		}
	}

	// Reset the render target back to the original back buffer and not the render buffers
	ID3D11RenderTargetView* renderTargets[1] = { mD3D->GetRenderTargetView() };
	mD3D->GetImmediateContext()->OMSetRenderTargets(1, renderTargets, mD3D->GetDepthStencilView());

	// Reset viewport
	mD3D->GetImmediateContext()->RSSetViewports(1, &mD3D->GetScreenViewport());
}
void GraphicsEngineImpl::UpdateSceneData()
{
	//--------------------------------------------------------
	// Compute scene bounding box
	//--------------------------------------------------------
	    XMFLOAT3 minPt(+MathHelper::infinity, +MathHelper::infinity, +MathHelper::infinity);
	    XMFLOAT3 maxPt(-MathHelper::infinity, -MathHelper::infinity, -MathHelper::infinity);
	 
	    // Get vertex positions from all models
	    for (UINT i = 0; i < mInstances.size(); ++i)
	    {
			for (UINT j = 0; j < mInstances[i]->model->meshes.size(); ++j)
	            {

					for (UINT k = 0; k < mInstances[i]->model->meshes.at(j).vertices.size(); ++k)
					{
						XMFLOAT3 vPos = mInstances[i]->model->meshes.at(j).vertices.at(k).position;
						minPt.x = MathHelper::getMin(minPt.x, vPos.x);
						minPt.y = MathHelper::getMin(minPt.x, vPos.x);
						minPt.z = MathHelper::getMin(minPt.x, vPos.x);

						maxPt.x = MathHelper::getMax(maxPt.x, vPos.x);
						maxPt.y = MathHelper::getMax(maxPt.x, vPos.x);
						maxPt.z = MathHelper::getMax(maxPt.x, vPos.x);
					}
	            }
	    }
	 
	    // Sphere center is at half of these new dimensions
	    mSceneBounds.Center = XMFLOAT3(        0.5f*(minPt.x + maxPt.x),
	            0.5f*(minPt.y + maxPt.y),
	            0.5f*(minPt.z + maxPt.z));
	 
	    // Calculate the sphere radius
	    XMFLOAT3 extent(0.5f*(maxPt.x - minPt.x),
	            0.5f*(maxPt.y - minPt.y),
	            0.5f*(maxPt.z - minPt.z));
	 
	    mSceneBounds.Radius = sqrtf(extent.x*extent.x + extent.y*extent.y + extent.z*extent.z);
}
