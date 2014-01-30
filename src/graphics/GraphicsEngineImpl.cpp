#include "GraphicsEngineImpl.h"
#include "Texture2DImpl.h"

GraphicsEngineImpl::GraphicsEngineImpl()
{
}

GraphicsEngineImpl::~GraphicsEngineImpl()
{
	if (mRandom1DTexSRV)
		ReleaseCOM(mRandom1DTexSRV);
	if (mParticlesTextureArray)
		ReleaseCOM(mParticlesTextureArray);

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
		/*if (mAnimatedInstances[i])
			delete mAnimatedInstances[i];*/
	}

	for (auto& it(mSkinnedModels.begin()); it != mSkinnedModels.end(); ++it)
	{
		if (it->second)
			SafeDelete(it->second);
	}
	mSkinnedModels.clear();

	for (UINT i = 0; i < mMorphModels.size(); ++i)
	{
		delete mMorphModels[i];
	}

	for (UINT i = 0; i < mMorphInstances.size(); ++i)
	{
		delete mMorphInstances[i];
	}

	for (UINT i = 0; i < mParticleSystems.size(); ++i)
	{
		delete mParticleSystems[i];
	}

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

	delete mIntermediateTexture;

	delete mSSAOTexture;
	delete mSSAOBlurTexture;

	delete mDoFCoCTexture;
	delete mDoFBlurTexture1;
	delete mDoFBlurTexture2;

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

	mMorphModels.push_back(new MorphModel(mD3D->GetDevice(), mTextureMgr, mResourceDir + "Models/Morphtest/Block/", "WoodBlock.morph"));
	
	/*
	mMorphInstances.push_back(new MorphModelInstance());
	XMMATRIX scaling = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX rotation = XMMatrixRotationY(0.0f);
	XMMATRIX offset = XMMatrixTranslation(20.0f, 20.0f, 0.0f);
	XMStoreFloat4x4(&mMorphInstances[0]->world, scaling*rotation*offset);
	mMorphInstances[0]->model = mMorphModels[0];
	mMorphInstances[0]->weights = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	morphTimeTest = 0.0f;
	morphIncrease = true;
	*/

	// Camera
	mCamera = new Camera();
	mCamera->SetLens(fovY, static_cast<float>(width) / height, zNear, zFar);
	mCamera->UpdateOrthoMatrix(static_cast<float>(width), static_cast<float>(height), zNear, zFar);
	mCamera->SetPosition(XMFLOAT3(0.0f, 0.0f, -400.0f));
	mCamera->UpdateBaseViewMatrix();
	mCamera->Update();

	mDeferredBuffers = new DeferredBuffers();
	mDeferredBuffers->Init(mD3D->GetDevice(), width, height);

	mIntermediateTexture = new Texture2DImpl(mD3D->GetDevice(), mD3D->GetImmediateContext(), width, height, DXGI_FORMAT_R8G8B8A8_UNORM, true);

	mSSAOScale = 1.0f;
	mSSAOTexture = new Texture2DImpl(mD3D->GetDevice(), mD3D->GetImmediateContext(), (UINT)(width * mSSAOScale), (UINT)(height * mSSAOScale), DXGI_FORMAT_R8_UNORM, true);
	mSSAOBlurTexture = new Texture2DImpl(mD3D->GetDevice(), mD3D->GetImmediateContext(), (UINT)(width * mSSAOScale), (UINT)(height * mSSAOScale), DXGI_FORMAT_R8_UNORM, true);

	// Also change Blur.hlsi if you change mDoFScale!
	mDoFScale = 0.5f;
	mDoFCoCTexture = new Texture2DImpl(mD3D->GetDevice(), mD3D->GetImmediateContext(), (UINT)(width * mDoFScale), (UINT)(height * mDoFScale), DXGI_FORMAT_R8_UNORM, true);
	mDoFBlurTexture1 = new Texture2DImpl(mD3D->GetDevice(), mD3D->GetImmediateContext(), (UINT)(width * mDoFScale), (UINT)(height * mDoFScale), DXGI_FORMAT_R8G8B8A8_UNORM, true);
	mDoFBlurTexture2 = new Texture2DImpl(mD3D->GetDevice(), mD3D->GetImmediateContext(), (UINT)(width * mDoFScale), (UINT)(height * mDoFScale), DXGI_FORMAT_R8G8B8A8_UNORM, true);
	mNearBlurryPlane = 10.0f;
	mNearSharpPlane = 75.0f;
	mFarSharpPlane = 200.0f;
	mFarBlurryPlane = 250.0f;

	mSky = new Sky(mD3D->GetDevice(), mTextureMgr, mResourceDir + "Textures\\SkyBox.dds", 2000.0f);
	mShadowMap = new ShadowMap(mD3D->GetDevice(), 2048, 2048);

	mGameTime = 0.0f;

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

	// Shadow mapping
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\ShadowBuildVS.cso", "ShadowBuildVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\ShadowBuildPS.cso", "ShadowBuildPS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\SkinnedShadowBuildVS.cso", "SkinnedShadowBuildVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\SkinnedShadowBuildPS.cso", "SkinnedShadowBuildPS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\ShadowBuildMorphVS.cso", "ShadowBuildMorphVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\ShadowBuildMorphPS.cso", "ShadowBuildMorphPS", mD3D->GetDevice());

	// Deferred shaders
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\BasicDeferredVS.cso", "BasicDeferredVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\BasicDeferredPS.cso", "BasicDeferredPS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\LightDeferredVS.cso", "LightDeferredVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\LightDeferredPS.cso", "LightDeferredPS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\BasicDeferredSkinnedVS.cso", "BasicDeferredSkinnedVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\BasicDeferredSkinnedPS.cso", "BasicDeferredSkinnedPS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\SkyDeferredVS.cso", "SkyDeferredVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\SkyDeferredPS.cso", "SkyDeferredPS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\BasicDeferredMorphVS.cso", "BasicDeferredMorphVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\BasicDeferredMorphPS.cso", "BasicDeferredMorphPS", mD3D->GetDevice());

	// Post-processing shaders
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\FullscreenQuadVS.cso", "FullscreenQuadVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\AmbientOcclusionPS.cso", "AmbientOcclusionPS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\SSAOBlurHorizontalPS.cso", "SSAOBlurHorizontalPS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\SSAOBlurVerticalPS.cso", "SSAOBlurVerticalPS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\DepthOfFieldCoCPS.cso", "DepthOfFieldCoCPS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\DepthOfFieldBlurHorizontalPS.cso", "DepthOfFieldBlurHorizontalPS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\DepthOfFieldBlurVerticalPS.cso", "DepthOfFieldBlurVerticalPS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\CompositePS.cso", "CompositePS", mD3D->GetDevice());

	// Particle system shaders
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\DrawParticleVS.cso", "DrawParticleVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledGeometryShader(L"..\\shaders\\DrawParticleGS.cso", "DrawParticleGS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\DrawParticlePS.cso", "DrawParticlePS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\StreamOutParticleVS.cso", "StreamOutParticleVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledGeometryStreamOutShader(L"..\\shaders\\StreamOutParticleGS.cso", "StreamOutParticleGS", mD3D->GetDevice(),
		GeoStreamOutDesc::ParticleSoDesc,
		COUNT_OF(GeoStreamOutDesc::ParticleSoDesc),
		D3D11_SO_NO_RASTERIZED_STREAM,
		&GeoStreamOutDesc::ParticleStride,
		1);

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
	mShaderHandler->mBasicDeferredSkinnedShader->BindShaders(
		mShaderHandler->GetVertexShader("BasicDeferredSkinnedVS"),
		mShaderHandler->GetPixelShader("BasicDeferredSkinnedPS"));
	mShaderHandler->mLightDeferredShader->BindShaders(
		mShaderHandler->GetVertexShader("LightDeferredVS"),
		mShaderHandler->GetPixelShader("LightDeferredPS"));
	mShaderHandler->mSkyDeferredShader->BindShaders(
		mShaderHandler->GetVertexShader("SkyDeferredVS"),
		mShaderHandler->GetPixelShader("SkyDeferredPS"));
	mShaderHandler->mSSAOShader->BindShaders(
		mShaderHandler->GetVertexShader("FullscreenQuadVS"),
		mShaderHandler->GetPixelShader("AmbientOcclusionPS"));
	mShaderHandler->mSSAOBlurHorizontalShader->BindShaders(
		mShaderHandler->GetVertexShader("FullscreenQuadVS"),
		mShaderHandler->GetPixelShader("SSAOBlurHorizontalPS"));
	mShaderHandler->mSSAOBlurVerticalShader->BindShaders(
		mShaderHandler->GetVertexShader("FullscreenQuadVS"),
		mShaderHandler->GetPixelShader("SSAOBlurVerticalPS"));
	mShaderHandler->mDepthOfFieldCoCShader->BindShaders(
		mShaderHandler->GetVertexShader("FullscreenQuadVS"),
		mShaderHandler->GetPixelShader("DepthOfFieldCoCPS"));
	mShaderHandler->mDepthOfFieldBlurHorizontalShader->BindShaders(
		mShaderHandler->GetVertexShader("FullscreenQuadVS"),
		mShaderHandler->GetPixelShader("DepthOfFieldBlurHorizontalPS"));
	mShaderHandler->mDepthOfFieldBlurVerticalShader->BindShaders(
		mShaderHandler->GetVertexShader("FullscreenQuadVS"),
		mShaderHandler->GetPixelShader("DepthOfFieldBlurVerticalPS"));
	mShaderHandler->mCompositeShader->BindShaders(
		mShaderHandler->GetVertexShader("FullscreenQuadVS"),
		mShaderHandler->GetPixelShader("CompositePS"));
	mShaderHandler->mDeferredMorphShader->BindShaders(
		mShaderHandler->GetVertexShader("BasicDeferredMorphVS"),
		mShaderHandler->GetPixelShader("BasicDeferredMorphPS"));

	// Shadow mapping
	mShaderHandler->mShadowShader->BindShaders(
		mShaderHandler->GetVertexShader("ShadowBuildVS"),
		mShaderHandler->GetPixelShader("ShadowBuildPS"));
	mShaderHandler->mSkinnedShadowShader->BindShaders(
		mShaderHandler->GetVertexShader("SkinnedShadowBuildVS"),
		mShaderHandler->GetPixelShader("SkinnedShadowBuildPS"));
	mShaderHandler->mShadowMorphShader->BindShaders(
		mShaderHandler->GetVertexShader("ShadowBuildMorphVS"),
		mShaderHandler->GetPixelShader("ShadowBuildMorphPS"));

	// Particle system
	mShaderHandler->mParticleSystemShader->BindShaders(
		mShaderHandler->GetVertexShader("StreamOutParticleVS"),
		mShaderHandler->GetGeometryShader("StreamOutParticleGS"),
		mShaderHandler->GetVertexShader("DrawParticleVS"),
		mShaderHandler->GetGeometryShader("DrawParticleGS"),
		mShaderHandler->GetPixelShader("DrawParticlePS"));

	// Now create all the input layouts
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("BasicVS"), InputLayoutDesc::PosNormalTex, COUNT_OF(InputLayoutDesc::PosNormalTex), &mInputLayouts->PosNormalTex);
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("SkyVS"), InputLayoutDesc::Position, COUNT_OF(InputLayoutDesc::Position), &mInputLayouts->Position);
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("NormalMapSkinnedVS"),
		InputLayoutDesc::PosNormalTexTanSkinned,
		COUNT_OF(InputLayoutDesc::PosNormalTexTanSkinned),
		&mInputLayouts->PosNormalTexTanSkinned);
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("LightDeferredVS"), InputLayoutDesc::PosTex, COUNT_OF(InputLayoutDesc::PosTex), &mInputLayouts->PosTex);
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("BasicDeferredMorphVS"), InputLayoutDesc::PosNormalTexTargets4, COUNT_OF(InputLayoutDesc::PosNormalTexTargets4), &mInputLayouts->PosNormalTexTargets4);
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("StreamOutParticleVS"), InputLayoutDesc::Particle, COUNT_OF(InputLayoutDesc::Particle), &mInputLayouts->Particle);

	// Init all the shader objects
	mShaderHandler->mBasicShader->Init(mD3D->GetDevice(), mInputLayouts->PosNormalTex);
	mShaderHandler->mSkyShader->Init(mD3D->GetDevice(), mInputLayouts->Position);
	mShaderHandler->mNormalSkinned->Init(mD3D->GetDevice(), mInputLayouts->PosNormalTexTanSkinned);
	mShaderHandler->mBasicDeferredShader->Init(mD3D->GetDevice(), mInputLayouts->PosNormalTex);
	mShaderHandler->mBasicDeferredSkinnedShader->Init(mD3D->GetDevice(), mInputLayouts->PosNormalTexTanSkinned);
	mShaderHandler->mLightDeferredShader->Init(mD3D->GetDevice(), mInputLayouts->PosTex);
	mShaderHandler->mShadowShader->Init(mD3D->GetDevice(), mInputLayouts->PosNormalTex);
	mShaderHandler->mSkinnedShadowShader->Init(mD3D->GetDevice(), mInputLayouts->PosNormalTexTanSkinned);
	mShaderHandler->mSkyDeferredShader->Init(mD3D->GetDevice(), mInputLayouts->Position);
	mShaderHandler->mSSAOShader->Init(mD3D->GetDevice(), NULL);
	mShaderHandler->mSSAOBlurHorizontalShader->Init(mD3D->GetDevice(), NULL);
	mShaderHandler->mSSAOBlurVerticalShader->Init(mD3D->GetDevice(), NULL);
	mShaderHandler->mDepthOfFieldCoCShader->Init(mD3D->GetDevice(), NULL);
	mShaderHandler->mDepthOfFieldBlurHorizontalShader->Init(mD3D->GetDevice(), NULL);
	mShaderHandler->mDepthOfFieldBlurVerticalShader->Init(mD3D->GetDevice(), NULL);
	mShaderHandler->mCompositeShader->Init(mD3D->GetDevice(), NULL);
	mShaderHandler->mDeferredMorphShader->Init(mD3D->GetDevice(), mInputLayouts->PosNormalTexTargets4);
	mShaderHandler->mShadowMorphShader->Init(mD3D->GetDevice(), mInputLayouts->PosNormalTexTargets4);
	mShaderHandler->mParticleSystemShader->Init(mD3D->GetDevice(), mInputLayouts->Particle);

	std::string fontPath = mResourceDir + "myfile.spritefont";
	std::wstring fontPathW(fontPath.begin(), fontPath.end());

	mSpriteBatch = new SpriteBatch(mD3D->GetImmediateContext());
	mSpriteFont = new SpriteFont(mD3D->GetDevice(), fontPathW.c_str());

	// Create orthogonal window
	mOrthoWindow = new OrthoWindow();
	mOrthoWindow->Initialize(mD3D->GetDevice(), width, height);

	mPostProcessingEffects = POST_PROCESSING_SSAO | POST_PROCESSING_DOF;

	// PARTICLE SYSTEM TEST
	/*
	mRandom1DTexSRV = d3dHelper::CreateRandomTexture1DSRV(mD3D->GetDevice());

	LoadParticles(mResourceDir + "Textures/Particles/", "Particles.particlelist");

	ParticleSystem* testSystem1 = new ParticleSystem();
	testSystem1->Init(
		mD3D->GetDevice(),
		mShaderHandler->mParticleSystemShader,
		mParticlesTextureArray,
		mRandom1DTexSRV,
		1000);
	testSystem1->SetEmitPos(XMFLOAT3(0.0f, 15.0f, 0.0f));
	testSystem1->SetConstantAccel(XMFLOAT3(0.0f, 7.8f, 0.0f));
	testSystem1->SetParticleType(ParticleType::PT_FLARE1);
	mParticleSystems.push_back(testSystem1);
	*/

	return true;
}

CameraController* GraphicsEngineImpl::CreateCameraController()
{
	return CreateCameraControll(mCamera);
}

void GraphicsEngineImpl::DeleteCameraController(CameraController *controller)
{
	DestroyCameraController(controller);
}

void GraphicsEngineImpl::Run(float dt)
{
	UpdateScene(dt, mGameTime);
	DrawScene();
}

void GraphicsEngineImpl::DrawScene()
{
	// Restore default states
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	mD3D->GetImmediateContext()->RSSetState(0);
	mD3D->GetImmediateContext()->OMSetDepthStencilState(0, 0);
	mD3D->GetImmediateContext()->OMSetBlendState(0, blendFactor, 0xffffffff);

	// Draw scene to shadowmap
	mD3D->GetImmediateContext()->RSSetState(RenderStates::mDepthBiasRS); // This rasterizer state fixes shadow acne
	mShadowMap->BindDsvAndSetNullRenderTarget(mD3D->GetImmediateContext());
	mShadowMap->BuildShadowTransform(mDirLights.at(0), mSceneBounds);
	//mShadowMap->DrawSceneToShadowMap(mInstances, mAnimatedInstances, mD3D->GetImmediateContext(), mShaderHandler->mShadowShader, mShaderHandler->mSkinnedShadowShader);
	mShadowMap->DrawSceneToShadowMap(mInstances, mAnimatedInstances, mMorphInstances, mD3D->GetImmediateContext(), mShaderHandler->mShadowShader, mShaderHandler->mSkinnedShadowShader, mShaderHandler->mShadowMorphShader);

	mD3D->GetImmediateContext()->RSSetState(0);
	// Restore back and depth buffer and viewport to the OM stage
	ID3D11RenderTargetView* renderTargets[1] = { mD3D->GetRenderTargetView() };
	mD3D->GetImmediateContext()->OMSetRenderTargets(1, renderTargets, mD3D->GetDepthStencilView());
	mD3D->GetImmediateContext()->ClearRenderTargetView(mD3D->GetRenderTargetView(), reinterpret_cast<const float*>(&D3dColors::LightSteelBlue));

	mD3D->GetImmediateContext()->ClearDepthStencilView(mD3D->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	mD3D->GetImmediateContext()->RSSetViewports(1, &mD3D->GetScreenViewport());

	//------------------------------------------------------------------------------
	// Deferred shading
	//------------------------------------------------------------------------------
	// Render the scene to the render buffers
	RenderSceneToTexture();

	// Turn off Z-buffer to begin 2D-drawing
	mD3D->GetImmediateContext()->OMSetDepthStencilState(RenderStates::mDisabledDSS, 1);

	ID3D11RenderTargetView* renderTarget;

	if (mPostProcessingEffects & POST_PROCESSING_SSAO)
	{
		D3D11_VIEWPORT SSAOViewport;
		SSAOViewport.TopLeftX = 0;
		SSAOViewport.TopLeftY = 0;
		SSAOViewport.Width = (float)mSSAOTexture->GetWidth();
		SSAOViewport.Height = (float)mSSAOTexture->GetHeight();
		SSAOViewport.MinDepth = 0.0f;
		SSAOViewport.MaxDepth = 1.0f;

		mD3D->GetImmediateContext()->RSSetViewports(1, &SSAOViewport);

		// Calculate ambient occlusion
		renderTarget = mSSAOTexture->GetRenderTargetView();
		mD3D->GetImmediateContext()->OMSetRenderTargets(1, &renderTarget, NULL);

		mShaderHandler->mSSAOShader->SetDepthTexture(mD3D->GetImmediateContext(), mD3D->GetDepthStencilSRView());
		mShaderHandler->mSSAOShader->SetNormalTexture(mD3D->GetImmediateContext(), mDeferredBuffers->GetSRV(DeferredBuffersIndex::Normal));
		mShaderHandler->mSSAOShader->SetRandomTexture(mD3D->GetImmediateContext(), mTextureMgr->CreateTexture(mResourceDir + "Textures/random.png"));

		mShaderHandler->mSSAOShader->SetInverseProjectionMatrix(XMMatrixInverse(nullptr, mCamera->GetProjMatrix()));
		mShaderHandler->mSSAOShader->SetViewMatrix(mCamera->GetViewMatrix());
		mShaderHandler->mSSAOShader->SetZFar(zFar);
		mShaderHandler->mSSAOShader->SetParameters(mSSAOradius, mSSAOprojectionFactor, mSSAObias, mSSAOcontrast, mSSAOsigma);
		mShaderHandler->mSSAOShader->Update(mD3D->GetImmediateContext());

		mShaderHandler->mSSAOShader->SetActive(mD3D->GetImmediateContext());

		// Render a fullscreen quad without a vertex buffer using some shader magic.
		mD3D->GetImmediateContext()->Draw(3, 0);

		mShaderHandler->mSSAOShader->SetDepthTexture(mD3D->GetImmediateContext(), NULL);
		mShaderHandler->mSSAOShader->SetNormalTexture(mD3D->GetImmediateContext(), NULL);
		mShaderHandler->mSSAOShader->SetRandomTexture(mD3D->GetImmediateContext(), NULL);
	
		// Horizontal blur
		renderTarget = mSSAOBlurTexture->GetRenderTargetView();
		mD3D->GetImmediateContext()->OMSetRenderTargets(1, &renderTarget, NULL);

		mShaderHandler->mSSAOBlurHorizontalShader->SetFramebufferTexture(mD3D->GetImmediateContext(), mSSAOTexture->GetShaderResourceView());
		mShaderHandler->mSSAOBlurHorizontalShader->SetInputTexture(mD3D->GetImmediateContext(), mD3D->GetDepthStencilSRView());

		mShaderHandler->mSSAOBlurHorizontalShader->SetFramebufferSize(XMFLOAT2((float)mSSAOTexture->GetWidth(), (float)mSSAOTexture->GetHeight()));
		mShaderHandler->mSSAOBlurHorizontalShader->SetZNearFar(zNear, zFar);
		mShaderHandler->mSSAOBlurHorizontalShader->Update(mD3D->GetImmediateContext());

		mShaderHandler->mSSAOBlurHorizontalShader->SetActive(mD3D->GetImmediateContext());

		// Render a fullscreen quad without a vertex buffer using some shader magic.
		mD3D->GetImmediateContext()->Draw(3, 0);

		mShaderHandler->mSSAOBlurHorizontalShader->SetFramebufferTexture(mD3D->GetImmediateContext(), NULL);
		mShaderHandler->mSSAOBlurHorizontalShader->SetInputTexture(mD3D->GetImmediateContext(), NULL);

		// Vertical blur
		renderTarget = mSSAOTexture->GetRenderTargetView();
		mD3D->GetImmediateContext()->OMSetRenderTargets(1, &renderTarget, NULL);

		mShaderHandler->mSSAOBlurVerticalShader->SetFramebufferTexture(mD3D->GetImmediateContext(), mSSAOBlurTexture->GetShaderResourceView());
		mShaderHandler->mSSAOBlurVerticalShader->SetInputTexture(mD3D->GetImmediateContext(), mD3D->GetDepthStencilSRView());

		mShaderHandler->mSSAOBlurVerticalShader->SetFramebufferSize(XMFLOAT2((float)mSSAOTexture->GetWidth(), (float)mSSAOTexture->GetHeight()));
		mShaderHandler->mSSAOBlurVerticalShader->SetZNearFar(zNear, zFar);
		mShaderHandler->mSSAOBlurVerticalShader->Update(mD3D->GetImmediateContext());

		mShaderHandler->mSSAOBlurVerticalShader->SetActive(mD3D->GetImmediateContext());

		// Render a fullscreen quad without a vertex buffer using some shader magic.
		mD3D->GetImmediateContext()->Draw(3, 0);

		mShaderHandler->mSSAOBlurVerticalShader->SetFramebufferTexture(mD3D->GetImmediateContext(), NULL);
		mShaderHandler->mSSAOBlurVerticalShader->SetInputTexture(mD3D->GetImmediateContext(), NULL);
	}
	else
	{
		float clearColor[4] = {1.0, 1.0, 1.0, 1.0};
		mD3D->GetImmediateContext()->ClearRenderTargetView(mSSAOTexture->GetRenderTargetView(), clearColor);
	}

	renderTarget = mIntermediateTexture->GetRenderTargetView();
	mD3D->GetImmediateContext()->OMSetRenderTargets(1, &renderTarget, NULL);
 	mD3D->GetImmediateContext()->RSSetViewports(1, &mD3D->GetScreenViewport());

	mShaderHandler->mLightDeferredShader->SetActive(mD3D->GetImmediateContext());
	mShaderHandler->mLightDeferredShader->SetEyePosW(mCamera->GetPosition());
	mShaderHandler->mLightDeferredShader->SetPointLights(mD3D->GetImmediateContext(), (UINT)mPointLights.size(), mPointLights.data());
	mShaderHandler->mLightDeferredShader->SetDirLights(mD3D->GetImmediateContext(), (UINT)mDirLights.size(), mDirLights.data());
	mShaderHandler->mLightDeferredShader->SetSpotLights(mD3D->GetImmediateContext(), (UINT)mSpotLights.size(), mSpotLights.data());
	mShaderHandler->mLightDeferredShader->SetShadowTransform(mShadowMap->GetShadowTransform());
	mShaderHandler->mLightDeferredShader->SetCameraViewProjMatrix(mCamera->GetViewMatrix(), mCamera->GetProjMatrix());
	mShaderHandler->mLightDeferredShader->SetLightWorldViewProj(mShadowMap->GetLightWorld(), mShadowMap->GetLightView(), mShadowMap->GetLightProj());
	
	mShaderHandler->mLightDeferredShader->SetFogProperties(1, 0.0075f, -150.0f, 0.005f, XMFLOAT4(0.85f, 0.85f, 0.85f, 1.0f));

	mShaderHandler->mLightDeferredShader->UpdatePerFrame(mD3D->GetImmediateContext());

	mShaderHandler->mLightDeferredShader->SetDiffuseTexture(mD3D->GetImmediateContext(), mDeferredBuffers->GetSRV(DeferredBuffersIndex::Diffuse));
	mShaderHandler->mLightDeferredShader->SetNormalTexture(mD3D->GetImmediateContext(), mDeferredBuffers->GetSRV(DeferredBuffersIndex::Normal));
	mShaderHandler->mLightDeferredShader->SetSpecularTexture(mD3D->GetImmediateContext(), mDeferredBuffers->GetSRV(DeferredBuffersIndex::Specular));
	mShaderHandler->mLightDeferredShader->SetVelocityTexture(mD3D->GetImmediateContext(), mDeferredBuffers->GetSRV(DeferredBuffersIndex::Velocity));
	mShaderHandler->mLightDeferredShader->SetSSAOTexture(mD3D->GetImmediateContext(), mSSAOTexture->GetShaderResourceView());
	mShaderHandler->mLightDeferredShader->SetDepthTexture(mD3D->GetImmediateContext(), mD3D->GetDepthStencilSRView());

	mShaderHandler->mLightDeferredShader->SetWorldViewProj(XMMatrixIdentity(), mCamera->GetBaseViewMatrix(), mCamera->GetOrthoMatrix());
	mShaderHandler->mLightDeferredShader->UpdatePerObj(mD3D->GetImmediateContext());

	// Now render the window
	mOrthoWindow->Render(mD3D->GetImmediateContext());

	// Lastly, clear (unbind) the textures (otherwise D3D11 WARNING)
	mShaderHandler->mLightDeferredShader->SetDiffuseTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mLightDeferredShader->SetNormalTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mLightDeferredShader->SetSpecularTexture(mD3D->GetImmediateContext(), NULL);
	//mShaderHandler->mLightDeferredShader->SetPositionTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mLightDeferredShader->SetSSAOTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mLightDeferredShader->SetDepthTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mLightDeferredShader->SetVelocityTexture(mD3D->GetImmediateContext(), NULL);

	if (mPostProcessingEffects & POST_PROCESSING_DOF)
	{
		D3D11_VIEWPORT DoFViewport;
		DoFViewport.TopLeftX = 0;
		DoFViewport.TopLeftY = 0;
		DoFViewport.Width = (float)mDoFCoCTexture->GetWidth();
		DoFViewport.Height = (float)mDoFCoCTexture->GetHeight();
		DoFViewport.MinDepth = 0.0f;
		DoFViewport.MaxDepth = 1.0f;

		mD3D->GetImmediateContext()->RSSetViewports(1, &DoFViewport);

		// Calculate the circle of confusion.
		renderTarget = mDoFCoCTexture->GetRenderTargetView();
		mD3D->GetImmediateContext()->OMSetRenderTargets(1, &renderTarget, NULL);

		mShaderHandler->mDepthOfFieldCoCShader->SetDepthTexture(mD3D->GetImmediateContext(), mD3D->GetDepthStencilSRView());

		mShaderHandler->mDepthOfFieldCoCShader->SetZNearFar(zNear, zFar);
		mShaderHandler->mDepthOfFieldCoCShader->SetFocusPlanes(mNearBlurryPlane, mNearSharpPlane, mFarSharpPlane, mFarBlurryPlane);
		mShaderHandler->mDepthOfFieldCoCShader->Update(mD3D->GetImmediateContext());

		mShaderHandler->mDepthOfFieldCoCShader->SetActive(mD3D->GetImmediateContext());

		// Render a fullscreen quad without a vertex buffer using some shader magic.
		mD3D->GetImmediateContext()->Draw(3, 0);

		mShaderHandler->mDepthOfFieldCoCShader->SetDepthTexture(mD3D->GetImmediateContext(), NULL);

		// Horizontal blur
		renderTarget = mDoFBlurTexture1->GetRenderTargetView();
		mD3D->GetImmediateContext()->OMSetRenderTargets(1, &renderTarget, NULL);

		mShaderHandler->mDepthOfFieldBlurHorizontalShader->SetFramebufferTexture(mD3D->GetImmediateContext(), mIntermediateTexture->GetShaderResourceView());
		mShaderHandler->mDepthOfFieldBlurHorizontalShader->SetInputTexture(mD3D->GetImmediateContext(), mDoFCoCTexture->GetShaderResourceView());

		mShaderHandler->mDepthOfFieldBlurHorizontalShader->SetFramebufferSize(XMFLOAT2((float)mDoFCoCTexture->GetWidth(), (float)mDoFCoCTexture->GetHeight()));
		mShaderHandler->mDepthOfFieldBlurHorizontalShader->SetZNearFar(zNear, zFar);
		mShaderHandler->mDepthOfFieldBlurHorizontalShader->Update(mD3D->GetImmediateContext());

		mShaderHandler->mDepthOfFieldBlurHorizontalShader->SetActive(mD3D->GetImmediateContext());

		// Render a fullscreen quad without a vertex buffer using some shader magic.
		mD3D->GetImmediateContext()->Draw(3, 0);

		mShaderHandler->mDepthOfFieldBlurHorizontalShader->SetFramebufferTexture(mD3D->GetImmediateContext(), NULL);
		mShaderHandler->mDepthOfFieldBlurHorizontalShader->SetInputTexture(mD3D->GetImmediateContext(), NULL);

		// Vertical blur
		renderTarget = mDoFBlurTexture2->GetRenderTargetView();
		mD3D->GetImmediateContext()->OMSetRenderTargets(1, &renderTarget, NULL);

		mShaderHandler->mDepthOfFieldBlurVerticalShader->SetFramebufferTexture(mD3D->GetImmediateContext(), mDoFBlurTexture1->GetShaderResourceView());
		mShaderHandler->mDepthOfFieldBlurVerticalShader->SetInputTexture(mD3D->GetImmediateContext(), mDoFCoCTexture->GetShaderResourceView());

		mShaderHandler->mDepthOfFieldBlurVerticalShader->SetFramebufferSize(XMFLOAT2((float)mDoFCoCTexture->GetWidth(), (float)mDoFCoCTexture->GetHeight()));
		mShaderHandler->mDepthOfFieldBlurVerticalShader->SetZNearFar(zNear, zFar);
		mShaderHandler->mDepthOfFieldBlurVerticalShader->Update(mD3D->GetImmediateContext());

		mShaderHandler->mDepthOfFieldBlurVerticalShader->SetActive(mD3D->GetImmediateContext());

		// Render a fullscreen quad without a vertex buffer using some shader magic.
		mD3D->GetImmediateContext()->Draw(3, 0);

		mShaderHandler->mDepthOfFieldBlurVerticalShader->SetFramebufferTexture(mD3D->GetImmediateContext(), NULL);
		mShaderHandler->mDepthOfFieldBlurVerticalShader->SetInputTexture(mD3D->GetImmediateContext(), NULL);

		// Reset the render target to the back buffer.
		renderTarget = mD3D->GetRenderTargetView();
		mD3D->GetImmediateContext()->OMSetRenderTargets(1, &renderTarget, NULL);
		// Reset viewport
 		mD3D->GetImmediateContext()->RSSetViewports(1, &mD3D->GetScreenViewport());

		// Composite the result of the light pass with the depth of field.
		mShaderHandler->mCompositeShader->SetFramebufferTexture(mD3D->GetImmediateContext(), mIntermediateTexture->GetShaderResourceView());
		mShaderHandler->mCompositeShader->SetDoFCoCTexture(mD3D->GetImmediateContext(), mDoFCoCTexture->GetShaderResourceView());
		mShaderHandler->mCompositeShader->SetDoFFarFieldTexture(mD3D->GetImmediateContext(), mDoFBlurTexture2->GetShaderResourceView());

		mShaderHandler->mCompositeShader->Update(mD3D->GetImmediateContext());
		mShaderHandler->mCompositeShader->SetActive(mD3D->GetImmediateContext());

		// Render a fullscreen quad without a vertex buffer using some shader magic.
		mD3D->GetImmediateContext()->Draw(3, 0);

		mShaderHandler->mCompositeShader->SetFramebufferTexture(mD3D->GetImmediateContext(), NULL);
		mShaderHandler->mCompositeShader->SetDoFCoCTexture(mD3D->GetImmediateContext(), NULL);
		mShaderHandler->mCompositeShader->SetDoFFarFieldTexture(mD3D->GetImmediateContext(), NULL);
	}
	else
	{
		ID3D11Resource *source = mIntermediateTexture->GetTexture();
		ID3D11Resource *destination;
		mD3D->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&destination));

		// Copy the result of the light pass to the backbuffer.
		mD3D->GetImmediateContext()->CopyResource(destination, source);
	}

	//-------------------------------------------------------------------------------------
	// Motion blur cache
	//-------------------------------------------------------------------------------------
	// Store current view projection matrix as previous view proj for use in next frame
	XMStoreFloat4x4(&mPrevViewProjMatrix, mCamera->GetViewProjMatrix());

	// Loop through all model instances
	for (UINT i = 0; i < mInstances.size(); ++i)
	{
		if (mInstances[i]->IsVisible())
		{
			mInstances[i]->SetPrevWorld(mInstances[i]->GetWorld());
		}

	}

	//-------------------------------------------------------------------------------------
	// Restore defaults
	//-------------------------------------------------------------------------------------
	// Reset the render target to the back buffer.
	renderTarget = mD3D->GetRenderTargetView();
	mD3D->GetImmediateContext()->OMSetRenderTargets(1, &renderTarget, NULL);
	// Reset viewport
 	mD3D->GetImmediateContext()->RSSetViewports(1, &mD3D->GetScreenViewport());

	// Turn z-buffer back on
	mD3D->GetImmediateContext()->OMSetDepthStencilState(RenderStates::mDefaultDSS, 1);

	ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	mD3D->GetImmediateContext()->PSSetShaderResources(0, 16, nullSRV);
	mD3D->GetImmediateContext()->VSSetShaderResources(0, 16, nullSRV);

	mD3D->GetImmediateContext()->RSSetState(0);
	mD3D->GetImmediateContext()->OMSetDepthStencilState(0, 0);
	mD3D->GetImmediateContext()->OMSetBlendState(0, blendFactor, 0xffffffff);
}

void GraphicsEngineImpl::UpdateScene(float dt, float gameTime)
{
	mGameTime = gameTime;

	// Update skinned instances
	for (size_t i = 0; i < mAnimatedInstances.size(); i++)
	{
		mAnimatedInstances[i]->model->SetKeyFrameInterval(mAnimatedInstances[i]->model->mAnimations[mAnimatedInstances[i]->model->mCurAnim].FrameStart, mAnimatedInstances[i]->model->mAnimations[mAnimatedInstances[i]->model->mCurAnim].FrameEnd);
		mAnimatedInstances[i]->model->Update(dt);
	}

	// Morph testing
	/*
	if (mMorphInstances.size() >= 1)
	{
		if (mMorphInstances.front()->weights.x >= 1.0f)
			morphIncrease = false;

		if (mMorphInstances.front()->weights.x <= 0.0f)
			morphIncrease = true;

		if (morphIncrease)
			mMorphInstances.front()->weights.x += 2.0f * dt;
		else
			mMorphInstances.front()->weights.x -= 2.0f * dt;
	}
	*/

	for (UINT i = 0; i < mParticleSystems.size(); ++i)
	{
		mParticleSystems[i]->Update(dt, gameTime);
		mParticleSystems[i]->SetParticleType((rand() % ParticleType::NROFTYPES - 1) + 1);
	}

	mCamera->Update();
}

void GraphicsEngineImpl::Present()
{
	// Present the back buffer to front buffer
	// Set SyncInterval to 1 if you want to limit the FPS to the monitors refresh rate
	HR(mD3D->GetSwapChain()->Present(0, 0));
}

void GraphicsEngineImpl::Begin2D()
{
	mSpriteBatch->Begin();
}

void GraphicsEngineImpl::End2D()
{
	mSpriteBatch->End();
}

void GraphicsEngineImpl::Draw2DTextureFile(const std::string file, const Draw2DInput* input)
{
	mSpriteBatch->Draw(
		mTextureMgr->CreateTexture(file),
		input->pos,
		0,
		input->color,
		0.0f, 
		XMFLOAT2(0.0f, 0.0f),
		input->scale,
		SpriteEffects::SpriteEffects_None,
		input->layerDepth);
}

void GraphicsEngineImpl::Draw2DTexture(Texture2D *texture, const Draw2DInput* input)
{
	Texture2DImpl *textureImpl = (Texture2DImpl *)texture;
	mSpriteBatch->Draw(
		textureImpl->GetShaderResourceView(), 
		input->pos, 
		0, 
		D3dColors::White, 
		0.0f,
		XMFLOAT2(0.0f,0.0f),
		input->scale,
		SpriteEffects::SpriteEffects_None, 
		input->layerDepth);
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
			mResourceDir + ss.str());
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
			mResourceDir + ss.str());
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
		mAnimatedInstances.erase(mAnimatedInstances.begin() + index);

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
	Texture2DImpl *texture = new Texture2DImpl(mD3D->GetDevice(), mD3D->GetImmediateContext(), width, height, DXGI_FORMAT_R8G8B8A8_UNORM, false);
 
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
	mDeferredBuffers->OnResize(mD3D->GetDevice(), width, height);
	mCamera->SetLens(fovY, (float)width / height, zNear, zFar);
	mCamera->UpdateOrthoMatrix(static_cast<float>(width), static_cast<float>(height), zNear, zFar);
	mOrthoWindow->OnResize(mD3D->GetDevice(), width, height);

	// Resize Post-processing textures
	mIntermediateTexture->Resize(mD3D->GetDevice(), width, height);

	mSSAOTexture->Resize(mD3D->GetDevice(), (UINT)(width * mSSAOScale), (UINT)(height * mSSAOScale));
	mSSAOBlurTexture->Resize(mD3D->GetDevice(), (UINT)(width * mSSAOScale), (UINT)(height * mSSAOScale));

	mDoFCoCTexture->Resize(mD3D->GetDevice(), (UINT)(width * mDoFScale), (UINT)(height * mDoFScale));
	mDoFBlurTexture1->Resize(mD3D->GetDevice(), (UINT)(width * mDoFScale), (UINT)(height * mDoFScale));
	mDoFBlurTexture2->Resize(mD3D->GetDevice(), (UINT)(width * mDoFScale), (UINT)(height * mDoFScale));
}

void GraphicsEngineImpl::RenderSceneToTexture()
{
	mDeferredBuffers->SetRenderTargets(mD3D->GetImmediateContext(), mD3D->GetDepthStencilView());
	mD3D->GetImmediateContext()->RSSetViewports(1, &mD3D->GetScreenViewport());

	mDeferredBuffers->ClearRenderTargets(mD3D->GetImmediateContext(), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), mD3D->GetDepthStencilView());

	//mShaderHandler->mSkyDeferredShader->SetActive(mD3D->GetImmediateContext());
	mSky->Draw(mD3D->GetImmediateContext(), *mCamera, mShaderHandler->mSkyDeferredShader);

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	mD3D->GetImmediateContext()->RSSetState(0);
	mD3D->GetImmediateContext()->OMSetDepthStencilState(0, 0);
	mD3D->GetImmediateContext()->OMSetBlendState(0, blendFactor, 0xffffffff);

	//---------------------------------------------------------------------------------------
	// Static opaque objects
	//---------------------------------------------------------------------------------------
	mShaderHandler->mBasicDeferredShader->SetActive(mD3D->GetImmediateContext());

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	mShaderHandler->mBasicDeferredShader->SetShadowMap(mD3D->GetImmediateContext(), mShadowMap->getDepthMapSRV());
	//mShaderHandler->mBasicDeferredShader->SetViewProjMatrices(mCamera->GetViewProjMatrix(), XMLoadFloat4x4(&mPrevViewProjMatrix));

	// Loop through all model instances
	for (UINT i = 0; i < mInstances.size(); ++i)
	{
		if (mInstances[i]->IsVisible())
		{
			mShaderHandler->mBasicDeferredShader->SetWorldViewProjTex(mInstances[i]->GetWorld(),
				mCamera->GetViewProjMatrix(),
				toTexSpace);

			mShaderHandler->mBasicDeferredShader->SetPrevWorldViewProj(mInstances[i]->GetPrevWorld(),
				XMLoadFloat4x4(&mPrevViewProjMatrix));

			mShaderHandler->mBasicDeferredShader->SetShadowTransformLightViewProj(
				XMMatrixMultiply(mInstances[i]->GetWorld(), mShadowMap->GetShadowTransform()),
				mShadowMap->GetLightView(),
				mShadowMap->GetLightProj());

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

	//---------------------------------------------------------------------------------------
	// Skinned
	//---------------------------------------------------------------------------------------
	mShaderHandler->mBasicDeferredSkinnedShader->SetActive(mD3D->GetImmediateContext());

	mShaderHandler->mBasicDeferredSkinnedShader->SetShadowMapTexture(mD3D->GetImmediateContext(), mShadowMap->getDepthMapSRV());

	// Loop through all model instances
	for (UINT i = 0; i < mAnimatedInstances.size(); ++i)
	{
		mShaderHandler->mBasicDeferredSkinnedShader->SetShadowTransform(
						XMMatrixMultiply(mAnimatedInstances[i]->GetWorld(), mShadowMap->GetShadowTransform()));
		if (mAnimatedInstances[i]->IsVisible())
		{
			mAnimatedInstances[i]->model->Draw(mD3D->GetImmediateContext(), mCamera, mShaderHandler->mBasicDeferredSkinnedShader, mAnimatedInstances[i]->GetWorld());
		}
	}

	//---------------------------------------------------------------------------------------
	// Morphed objects
	//---------------------------------------------------------------------------------------
	mShaderHandler->mDeferredMorphShader->SetActive(mD3D->GetImmediateContext());
	mShaderHandler->mDeferredMorphShader->SetShadowMapTexture(mD3D->GetImmediateContext(), mShadowMap->getDepthMapSRV());

	for (UINT i = 0; i < mMorphInstances.size(); ++i)
	{
		if (mMorphInstances[i]->isVisible)
		{
			mShaderHandler->mDeferredMorphShader->SetShadowTransform(
				XMMatrixMultiply(XMLoadFloat4x4(&mMorphInstances[i]->world), mShadowMap->GetShadowTransform()));

			mShaderHandler->mDeferredMorphShader->SetWorldViewProjTex(XMLoadFloat4x4(&mMorphInstances[i]->world),
				mCamera->GetViewProjMatrix(),
				toTexSpace);

			mShaderHandler->mDeferredMorphShader->SetWeights(mMorphInstances[i]->weights);
			mD3D->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			for (UINT j = 0; j < mMorphInstances[i]->model->mTargetModels.begin()->nrOfMeshes; ++j)
			{
				UINT matIndex = mMorphInstances[i]->model->mTargetModels.begin()->meshes[j].MaterialIndex;
				mShaderHandler->mDeferredMorphShader->SetMaterial(mMorphInstances[i]->model->mat[matIndex]);
				mShaderHandler->mDeferredMorphShader->SetDiffuseMap(mD3D->GetImmediateContext(), mMorphInstances[i]->model->diffuseMapSRV[matIndex]);
				mShaderHandler->mDeferredMorphShader->UpdatePerObj(mD3D->GetImmediateContext());

				mMorphInstances[i]->model->Draw(mD3D->GetImmediateContext());
			}
		}
	}

	//---------------------------------------------------------------------------------------
	// Particles
	//---------------------------------------------------------------------------------------
	//mD3D->GetImmediateContext()->RSSetState(0);
	for (UINT i = 0; i < mParticleSystems.size(); ++i)
	{
		mParticleSystems[i]->SetEyePos(mCamera->GetPosition());
		mParticleSystems[i]->Draw(mD3D->GetImmediateContext(), *mCamera);
	}

	// Restore to not using a geometry shader
	mD3D->GetImmediateContext()->GSSetShader(nullptr, nullptr, 0);

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
						vPos.x += mInstances[i]->GetPosition().X;
						vPos.y += mInstances[i]->GetPosition().Y;
						vPos.z += mInstances[i]->GetPosition().Z;

						minPt.x = MathHelper::getMin(minPt.x, vPos.x);
						minPt.y = MathHelper::getMin(minPt.y, vPos.y);
						minPt.z = MathHelper::getMin(minPt.z, vPos.z);

						maxPt.x = MathHelper::getMax(maxPt.x, vPos.x);
						maxPt.y = MathHelper::getMax(maxPt.y, vPos.y);
						maxPt.z = MathHelper::getMax(maxPt.z, vPos.z);
					}
	            }
	    }
	 
	    // Sphere center is at half of these new dimensions
	    mSceneBounds.Center = XMFLOAT3(
				0.5f*(minPt.x + maxPt.x),
	            0.5f*(minPt.y + maxPt.y),
	            0.5f*(minPt.z + maxPt.z));
	 
	    // Calculate the sphere radius
	    XMFLOAT3 extent(
				0.5f*(maxPt.x - minPt.x),
	            0.5f*(maxPt.y - minPt.y),
	            0.5f*(maxPt.z - minPt.z));
	 
	    mSceneBounds.Radius = sqrtf(extent.x*extent.x + extent.y*extent.y + extent.z*extent.z);
}

void GraphicsEngineImpl::addDirLight(Vec3 color, Vec3 direction, float intensity)
{
	DirectionalLight dirLight;
	ZeroMemory(&dirLight, sizeof(DirectionalLight));

	dirLight.Ambient = XMFLOAT4(intensity / 10.0f, intensity / 10.0f, intensity / 10.0f, 1.0f);
	dirLight.Diffuse = XMFLOAT4(color.X, color.Y, color.Z, 1.0f);
	dirLight.Direction = XMFLOAT3(direction.X, direction.Y, direction.Z);
	dirLight.Specular = XMFLOAT4(color.X, color.Y, color.Z, 1.0f);

	mDirLights.push_back(dirLight);
}

void GraphicsEngineImpl::addPointLight(Vec3 color, Vec3 position, float intensity)
{
	PointLight pointLight;
	ZeroMemory(&pointLight, sizeof(PointLight));

	pointLight.Position = XMFLOAT3(position.X, position.Y, position.Z);
	pointLight.Ambient = XMFLOAT4(color.X, color.Y, color.Z, 1.0f);
	pointLight.Specular = XMFLOAT4(color.X, color.Y, color.Z, 1.0f);
	pointLight.Diffuse = XMFLOAT4(color.X, color.Y, color.Z, 1.0f);
	pointLight.Attenuation = XMFLOAT3(intensity / 100.0f, intensity / 100.0f, intensity / 100.0f);
	pointLight.Range = 1000.0f;
	mPointLights.push_back(pointLight);
}

void GraphicsEngineImpl::addSpotLight(Vec3 color, Vec3 direction, Vec3 position, float angle)
{
	SpotLight spotLight;
	ZeroMemory(&spotLight, sizeof(SpotLight));

	spotLight.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	spotLight.Diffuse = XMFLOAT4(color.X, color.Y, color.Z, 1.0f);
	spotLight.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	spotLight.Attenuation = XMFLOAT3(1.0f, 0.0f, 0.0f);
	spotLight.Spot = angle;
	spotLight.Range = 1000.0f;
	spotLight.Position = XMFLOAT3(position.X, position.Y, position.Z);
	spotLight.Direction = XMFLOAT3(direction.X, direction.Y, direction.Z);
	mSpotLights.push_back(spotLight);
}

void GraphicsEngineImpl::clearLights()
{
	mSpotLights.clear();
	mDirLights.clear();
	mPointLights.clear();
}

void GraphicsEngineImpl::printText(wchar_t* text, int x, int y, Vec3 color, float scale)
{
	XMVECTORF32 v_color = { color.X, color.Y, color.Z, 1 };
	mSpriteFont->DrawString(mSpriteBatch, text, XMFLOAT2((float)x, (float)y), v_color, 0.0f, XMFLOAT2(0, 0), scale);
}
void GraphicsEngineImpl::Clear()
{
	mD3D->GetImmediateContext()->RSSetState(0);
	// Restore back and depth buffer and viewport to the OM stage
	ID3D11RenderTargetView* renderTargets[1] = { mD3D->GetRenderTargetView() };
	mD3D->GetImmediateContext()->OMSetRenderTargets(1, renderTargets, mD3D->GetDepthStencilView());
	mD3D->GetImmediateContext()->ClearRenderTargetView(mD3D->GetRenderTargetView(), reinterpret_cast<const float*>(&D3dColors::LightSteelBlue));

	mD3D->GetImmediateContext()->ClearDepthStencilView(mD3D->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	mD3D->GetImmediateContext()->RSSetViewports(1, &mD3D->GetScreenViewport());
}

unsigned int GraphicsEngineImpl::GetPostProcessingEffects()
{
	return mPostProcessingEffects;
}

void GraphicsEngineImpl::SetPostProcessingEffects(unsigned int effects)
{
	mPostProcessingEffects = effects;
}

void GraphicsEngineImpl::SetDepthOfFieldFocusPlanes(float nearBlurryPlane, float nearSharpPlane, float farSharpPlane, float farBlurryPlane)
{	
	mNearBlurryPlane = nearBlurryPlane;
	mNearSharpPlane = nearSharpPlane;
	mFarSharpPlane = farSharpPlane;
	mFarBlurryPlane = farBlurryPlane;
}

void GraphicsEngineImpl::SetSSAOParameters(float radius, float projection_factor, float bias, float contrast, float sigma)
{
	mSSAOradius = radius;
	mSSAOprojectionFactor = projection_factor;
	mSSAObias = bias;
	mSSAOcontrast = contrast;
	mSSAOsigma = sigma;
}

void GraphicsEngineImpl::LoadParticles(std::string particlesPath, std::string particleFileName)
{
	std::string particleFilePath = particlesPath + particleFileName;

	// Open corresponding particle file
	std::ifstream particleFile;
	particleFile.open(particleFilePath.c_str(), std::ios::in);

	if (!particleFile.is_open())
	{
		std::stringstream ErrorStream;
		ErrorStream << "Failed to load particle file " << particleFilePath;
		std::string ErrorStreamStr = ErrorStream.str();
		LPCSTR Text = ErrorStreamStr.c_str();
		MessageBoxA(0, Text, 0, 0);
	}
	else
	{
		std::vector<std::string> particles;
		std::string particle;

		while (particleFile >> particle)
		{
			particles.push_back(particlesPath + particle);
		}

		particleFile.close();

		mParticlesTextureArray = d3dHelper::CreateTexture2DArraySRV(mD3D->GetDevice(), mD3D->GetImmediateContext(), particles);
	}
}

void GraphicsEngineImpl::SetFullscreen(bool fullscreen)
{
	mD3D->GetSwapChain()->SetFullscreenState(fullscreen, NULL);
}

bool GraphicsEngineImpl::isFullscreen()
{
	BOOL fullscreen;
	mD3D->GetSwapChain()->GetFullscreenState(&fullscreen, NULL);
	return fullscreen == 1;
}
