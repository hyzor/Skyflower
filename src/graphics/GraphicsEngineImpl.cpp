#include "GraphicsEngineImpl.h"
#include "Texture2DImpl.h"
#include <algorithm>

// Must be included last!
#include "shared/debug.h"

GraphicsEngineImpl::GraphicsEngineImpl()
{
	mRandom1DTexSRV = nullptr;
	mParticlesTextureArray = nullptr;

	mPointLights = new PLight[MAXPLIGHTS];
	mDirLights = new DLight[MAXDLIGHTS];
	mSpotLights = new SLight[MAXSLIGHTS];

	mPointLightsCount = 0;
	mDirLightsCount = 0;
	mSpotLightsCount = 0;
}

GraphicsEngineImpl::~GraphicsEngineImpl()
{
	ReleaseCOM(mDepthStencilSRVCopy);
	ReleaseCOM(mDepthStencilTextureCopy);
	ReleaseCOM(mRandom1DTexSRV);

	for (UINT i = 0; i < mParticleSystems.size(); ++i)
	{
		delete mParticleSystems[i];
	}

	ReleaseCOM(mParticlesTextureArray);

	for (UINT i = 0; i < mSkinnedSortedInstances.size(); ++i)
	{
		delete mSkinnedSortedInstances[i];
	}

	for (auto& it(mSkinnedSortedModels.begin()); it != mSkinnedSortedModels.end(); ++it)
	{
		if (it->second)
			SafeDelete(it->second);
	}
	mSkinnedSortedModels.clear();

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

	for (UINT i = 0; i < mMorphInstances.size(); ++i)
	{
		delete mMorphInstances[i];
	}

	for (UINT i = 0; i < mMorphModels.size(); ++i)
	{
		delete mMorphModels[i];
	}

	mMaterials.clear();

	ReleaseCOM(mLineVertexBuffer);

	delete[] mPointLights;
	delete[] mDirLights;
	delete[] mSpotLights;

	delete mSky;
	delete mCascadedShadows;
	delete mCamera;

	mDeferredBuffers->Shutdown();
	delete mDeferredBuffers;

	mOrthoWindow->Shutdown();
	delete mOrthoWindow;

	delete mIntermediateTexture;

	delete mSSAOTexture;
	delete mSSAOBlurTexture;

	delete mDoFCoCTexture;
	delete mDoFBlurTexture1;
	delete mDoFBlurTexture2;

	mSMAA->Shutdown();
	delete mSMAA;

	delete mFullscreenTriangle;

	delete mShaderHandler;

	mInputLayouts->DestroyAll();
	delete mInputLayouts;
	RenderStates::DestroyAll();

	delete mSpriteFontMonospace;
	delete mSpriteFont;
	delete mSpriteBatch;

	delete mTextureMgr;

	mD3D->Shutdown();
	delete mD3D;
}

bool GraphicsEngineImpl::Init(HWND hWindow, UINT width, UINT height, const std::string &resourceDir)
{
	mScreenWidth = width;
	mScreenHeight = height;

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

	//mSky = new Sky(mD3D->GetDevice(), mTextureMgr, mResourceDir + "Textures\\SkyBox.dds", 2000.0f);
	mSky = new Sky(mD3D->GetDevice(), mTextureMgr, 2000.0f);
	mCascadedShadows = new CascadedShadows(mD3D->GetDevice(), 2048, 2048, 3); //Remember that a const is defined in LightDef.hlsli 
	mCascadedShadows->SetSplitMethod(FIT_TO_CASCADE);
	mCascadedShadows->SetNearFarFitMethod(FIT_NEARFAR_AABB);
	mCascadedShadows->SetSplitDepth(0.125f, 0);
	mCascadedShadows->SetSplitDepth(0.25f, 1);

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
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\LineVS.cso", "LineVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\LinePS.cso", "LinePS", mD3D->GetDevice());

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

	// Skinning shader with separated lower and upper body transformations
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\BasicDeferredSkinnedSortedVS.cso", "BasicDeferredSkinnedSortedVS", mD3D->GetDevice());

	// Lit scene to texture
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\LightDeferredPS_ToTexture.cso", "LightDeferredPS_ToTexture", mD3D->GetDevice());

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

	// SMAA shaders
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\SMAAEdgeDetectionVS.cso", "SMAAEdgeDetectionVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\SMAAColorEdgeDetectionPS.cso", "SMAAColorEdgeDetectionPS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\SMAADepthEdgeDetectionPS.cso", "SMAADepthEdgeDetectionPS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\SMAALumaEdgeDetectionPS.cso", "SMAALumaEdgeDetectionPS", mD3D->GetDevice());

	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\SMAABlendingWeightCalculationsVS.cso", "SMAABlendingWeightCalculationsVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\SMAABlendingWeightCalculationsPS.cso", "SMAABlendingWeightCalculationsPS", mD3D->GetDevice());

	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\SMAANeighborhoodBlendingVS.cso", "SMAANeighborhoodBlendingVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\SMAANeighborhoodBlendingPS.cso", "SMAANeighborhoodBlendingPS", mD3D->GetDevice());

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
	mShaderHandler->mLineShader->BindShaders(
		mShaderHandler->GetVertexShader("LineVS"),
		mShaderHandler->GetPixelShader("LinePS"));
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

	mShaderHandler->mLightDeferredToTextureShader->BindShaders(
		mShaderHandler->GetVertexShader("LightDeferredVS"),
		mShaderHandler->GetPixelShader("LightDeferredPS_ToTexture"));

	// Skinning shader with separated lower and upper body transformations
	mShaderHandler->mBasicDeferredSkinnedSortedShader->BindShaders(
		mShaderHandler->GetVertexShader("BasicDeferredSkinnedSortedVS"),
		mShaderHandler->GetPixelShader("BasicDeferredSkinnedPS"));

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

	// SMAA
	mShaderHandler->mSMAAColorEdgeDetectionShader->BindShaders(
		mShaderHandler->GetVertexShader("SMAAEdgeDetectionVS"),
		mShaderHandler->GetPixelShader("SMAAColorEdgeDetectionPS"));
	mShaderHandler->mSMAADepthEdgeDetectionShader->BindShaders(
		mShaderHandler->GetVertexShader("SMAAEdgeDetectionVS"),
		mShaderHandler->GetPixelShader("SMAADepthEdgeDetectionPS"));
	mShaderHandler->mSMAALumaEdgeDetectionShader->BindShaders(
		mShaderHandler->GetVertexShader("SMAAEdgeDetectionVS"),
		mShaderHandler->GetPixelShader("SMAALumaEdgeDetectionPS"));
	mShaderHandler->mSMAABlendingWeightCalculationsShader->BindShaders(
		mShaderHandler->GetVertexShader("SMAABlendingWeightCalculationsVS"),
		mShaderHandler->GetPixelShader("SMAABlendingWeightCalculationsPS"));
	mShaderHandler->mSMAANeighborhoodBlendingShader->BindShaders(
		mShaderHandler->GetVertexShader("SMAANeighborhoodBlendingVS"),
		mShaderHandler->GetPixelShader("SMAANeighborhoodBlendingPS"));

	// Now create all the input layouts
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("BasicVS"), InputLayoutDesc::PosNormalTex, COUNT_OF(InputLayoutDesc::PosNormalTex), &mInputLayouts->PosNormalTex);
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("LineVS"), InputLayoutDesc::Position2D, COUNT_OF(InputLayoutDesc::Position2D), &mInputLayouts->Position2D);
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
	mShaderHandler->mLineShader->Init(mD3D->GetDevice(), mInputLayouts->Position2D);
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

	mShaderHandler->mLightDeferredToTextureShader->Init(mD3D->GetDevice(), mInputLayouts->PosTex);

	mShaderHandler->mBasicDeferredSkinnedSortedShader->Init(mD3D->GetDevice(), mInputLayouts->PosNormalTexTanSkinned);

	// SMAA
	mShaderHandler->mSMAAColorEdgeDetectionShader->Init(mD3D->GetDevice(), mInputLayouts->PosTex);
	mShaderHandler->mSMAADepthEdgeDetectionShader->Init(mD3D->GetDevice(), mInputLayouts->PosTex);
	mShaderHandler->mSMAALumaEdgeDetectionShader->Init(mD3D->GetDevice(), mInputLayouts->PosTex);
	mShaderHandler->mSMAABlendingWeightCalculationsShader->Init(mD3D->GetDevice(), mInputLayouts->PosTex);
	mShaderHandler->mSMAANeighborhoodBlendingShader->Init(mD3D->GetDevice(), mInputLayouts->PosTex);

	mFullscreenTriangle = new FullscreenTriangle(mD3D->GetDevice(), mInputLayouts->PosTex);

	mSMAA = new SMAA();

	mSMAA->SetShaders(mShaderHandler->mSMAAColorEdgeDetectionShader,
		mShaderHandler->mSMAALumaEdgeDetectionShader,
		mShaderHandler->mSMAADepthEdgeDetectionShader,
		mShaderHandler->mSMAANeighborhoodBlendingShader,
		mShaderHandler->mSMAABlendingWeightCalculationsShader);

	mSMAA->Init(mD3D->GetDevice(), width, height,
		//mTextureMgr->CreateDDSTextureFromBytes(areaTexBytes, AREATEX_SIZE, "AreaTex"),
		//mTextureMgr->CreateLevelTexture(mResourceDir + "Textures/SMAA/AreaTexDX11.dds"),
		//mTextureMgr->CreateLevelTexture(mResourceDir + "Textures/SMAA/SearchTex.dds"),
		mTextureMgr->CreateStaticTexture(mResourceDir + "Textures/SMAA/AreaTexDX11.dds"),
		mTextureMgr->CreateStaticTexture(mResourceDir + "Textures/SMAA/SearchTex.dds"),
		mFullscreenTriangle);

	std::string fontPath = mResourceDir + "buxton.spritefont";
	std::string fontPathMonospace = mResourceDir + "monospace_8.spritefont";
	std::wstring fontPathW(fontPath.begin(), fontPath.end());
	std::wstring fontPatMonospacehW(fontPathMonospace.begin(), fontPathMonospace.end());

	mSpriteBatch = new SpriteBatch(mD3D->GetImmediateContext());
	mSpriteFont = new SpriteFont(mD3D->GetDevice(), fontPathW.c_str());
	mSpriteFontMonospace = new SpriteFont(mD3D->GetDevice(), fontPatMonospacehW.c_str());

	// Create orthogonal window
	mOrthoWindow = new OrthoWindow();
	mOrthoWindow->Initialize(mD3D->GetDevice(), width, height);

	mPostProcessingEffects = POST_PROCESSING_SSAO | POST_PROCESSING_DOF;

	// PARTICLE SYSTEM TEST
	mRandom1DTexSRV = d3dHelper::CreateRandomTexture1DSRV(mD3D->GetDevice());

	LoadParticles(mResourceDir + "Textures/Particles/", "Particles.particlelist");

	mEnableAntiAliasing = true;

	/*
	ParticleSystemImpl* testSystem1 = new ParticleSystemImpl();
	testSystem1->Init(
	mD3D->GetDevice(),
	mShaderHandler->mParticleSystemShader,
	mParticlesTextureArray,
	mRandom1DTexSRV,
	1000);
	testSystem1->SetEmitPos(XMFLOAT3(0.0f, -5.0f, 0.0f));
	testSystem1->SetConstantAccel(XMFLOAT3(0.0f, 7.8f, 0.0f));
	testSystem1->SetParticleType(ParticleType::PT_FLARE1);
	testSystem1->SetParticleAgeLimit(4.0f);
	testSystem1->SetParticleFadeTime(4.0f);
	testSystem1->SetScale(XMFLOAT2(5.0f, 5.0f));
	testSystem1->SetBlendingMethod(BlendingMethods::ALPHA_BLENDING);
	mParticleSystems.push_back(testSystem1);
	*/

	// Sorted skinned model test
	/*
	std::string testPath = mResourceDir + "Models/Player/playerAnimated.dae";
	mSkinnedSortedModels["TestSortedModel"] = new GenericSkinnedModelSorted(mD3D->GetDevice(), mTextureMgr, testPath);

	// Skinned model with separated upper and lower body transformations
	mSkinnedSortedInstances.push_back(new GenericSkinnedModelSortedInstance());
	mSkinnedSortedInstances.back()->model = mSkinnedSortedModels["TestSortedModel"];
	mSkinnedSortedInstances.back()->loopLowerBodyAnim = true;
	mSkinnedSortedInstances.back()->loopUpperbodyAnim = true;
	mSkinnedSortedInstances.back()->isVisible = true;
	mSkinnedSortedInstances.back()->playLowerBodyAnimForward = true;
	mSkinnedSortedInstances.back()->playUpperBodyAnimForward = true;
	mSkinnedSortedInstances.back()->lowerBodyFrameStart = 76;
	mSkinnedSortedInstances.back()->lowerBodyFrameEnd = 99 - 1;
	mSkinnedSortedInstances.back()->upperBodyFrameStart = 61;
	mSkinnedSortedInstances.back()->upperBodyFrameEnd = 75 - 1;
	mSkinnedSortedInstances.back()->AnimationIndex = 0;
	mSkinnedSortedInstances.back()->TimePos = 0.0f;

	XMMATRIX offset = XMMatrixTranslation(0.0f, 10.0f, 0.0f);
	XMMATRIX mrot = XMMatrixRotationX(0.0f);
	XMMATRIX mscale = XMMatrixScaling(1.0f, 1.0, 1.0f);
	XMMATRIX world = mscale*mrot*offset;
	XMStoreFloat4x4(&mSkinnedSortedInstances.back()->world, world);
	XMStoreFloat4x4(&mSkinnedSortedInstances.back()->prevWorld, world);
	*/

	HRESULT hr;

	// Depth/stencil buffer copy
	// Create the depth/stencil buffer and view
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = width;						// Texture width in texels
	depthStencilDesc.Height = height;					// Texture height in texels
	depthStencilDesc.MipLevels = 1;								// Number of mipmap levels
	depthStencilDesc.ArraySize = 1;								// Number of textures in texture array
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;	// Texel format

	// Set number of multisamples and quality level for the depth/stencil buffer
	// This has to match swap chain MSAA values
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;			// How the texture will be used
	depthStencilDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	// Where the resource will be bound to the pipeline
	depthStencilDesc.CPUAccessFlags = 0;					// Specify CPU access (Only GPU writes/reads to the depth/buffer)
	depthStencilDesc.MiscFlags = 0;							// Optional flags

	hr = mD3D->GetDevice()->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilTextureCopy);

	D3D11_SHADER_RESOURCE_VIEW_DESC depthStencilSRViewDesc;
	memset(&depthStencilSRViewDesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	depthStencilSRViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	depthStencilSRViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	depthStencilSRViewDesc.Texture2D.MipLevels = 1;

	hr = mD3D->GetDevice()->CreateShaderResourceView(mDepthStencilTextureCopy, &depthStencilSRViewDesc, &mDepthStencilSRVCopy);

	mCurFPS = 0.0f;
	mTargetFPS = 60.0f;


	mLineVertexBufferSize = 4096;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = (UINT)(sizeof(float)* 2 * mLineVertexBufferSize);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	mD3D->GetDevice()->CreateBuffer(&bufferDesc, NULL, &mLineVertexBuffer);

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

	mD3D->GetImmediateContext()->ClearDepthStencilView(mD3D->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ID3D11RenderTargetView* renderTarget;

	if (mDirLightsCount > 0)
	{
		//mShadowMap->BindDsvAndSetNullRenderTarget(mD3D->GetImmediateContext());
		//mShadowMap->BuildShadowTransform(mDirLights[0], mSceneBounds);
		//mShadowMap->DrawSceneToShadowMap(mInstances, mAnimatedInstances, mD3D->GetImmediateContext(), mShaderHandler->mShadowShader, mShaderHandler->mSkinnedShadowShader);
		//mShadowMap->DrawSceneToShadowMap(mInstances, mAnimatedInstances, mMorphInstances, mD3D->GetImmediateContext(), mShaderHandler->mShadowShader, mShaderHandler->mSkinnedShadowShader, mShaderHandler->mShadowMorphShader);

		//Draw scene to cascades
		mCascadedShadows->CreateLightFrustums(mDirLights[0], mSceneBounds, mSceneBB, mCamera);
		mCascadedShadows->RenderSceneToCascades(mInstances, mAnimatedInstances, mMorphInstances, mD3D->GetImmediateContext(), mShaderHandler->mShadowShader, mShaderHandler->mSkinnedShadowShader, mShaderHandler->mShadowMorphShader);
	}

	mD3D->GetImmediateContext()->RSSetState(0);
	// Restore back and depth buffer and viewport to the OM stage
	ID3D11RenderTargetView* renderTargets[1] = { mD3D->GetRenderTargetView() };
	mD3D->GetImmediateContext()->OMSetRenderTargets(1, renderTargets, mD3D->GetDepthStencilView());
	mD3D->GetImmediateContext()->ClearRenderTargetView(mD3D->GetRenderTargetView(), reinterpret_cast<const float*>(&Colors::LightSteelBlue));

	mD3D->GetImmediateContext()->ClearDepthStencilView(mD3D->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	mD3D->GetImmediateContext()->RSSetViewports(1, &mD3D->GetScreenViewport());

	// Render the scene to the render buffers and light it up
	RenderSceneToTexture();

	// Turn off Z-buffer to begin 2D-drawing
	//mD3D->GetImmediateContext()->OMSetDepthStencilState(RenderStates::mDisabledDSS, 0);
	mD3D->GetImmediateContext()->OMGetDepthStencilState(&RenderStates::mDepthStencilDisabledDSS, 0);

	//ID3D11RenderTargetView* renderTarget;

	//------------------------------------------------------------------------------
	// Post processing
	//------------------------------------------------------------------------------
	// SSAO
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
		mShaderHandler->mSSAOShader->SetRandomTexture(mD3D->GetImmediateContext(), mTextureMgr->CreateStaticTexture(mResourceDir + "Textures/random.png"));

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
		float clearColor[4] = { 1.0, 1.0, 1.0, 1.0 };
		mD3D->GetImmediateContext()->ClearRenderTargetView(mSSAOTexture->GetRenderTargetView(), clearColor);
	}

	ID3D11ShaderResourceView* finalSRV = mDeferredBuffers->GetLitSceneSRV();

	// Depth of field
	if (mPostProcessingEffects & POST_PROCESSING_DOF)
	{
		finalSRV = mIntermediateTexture->GetShaderResourceView();

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

		mShaderHandler->mDepthOfFieldBlurHorizontalShader->SetFramebufferTexture(mD3D->GetImmediateContext(), mDeferredBuffers->GetLitSceneSRV());
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

		// Set the render target to the intermediate texture.
		renderTarget = mIntermediateTexture->GetRenderTargetView();
		mD3D->GetImmediateContext()->OMSetRenderTargets(1, &renderTarget, NULL);
		// Reset viewport
		mD3D->GetImmediateContext()->RSSetViewports(1, &mD3D->GetScreenViewport());

		// Composite the result of the light pass with the depth of field.
		mShaderHandler->mCompositeShader->SetFramebufferTexture(mD3D->GetImmediateContext(), mDeferredBuffers->GetLitSceneSRV());
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

	// Anti-aliasing (SMAA)
	if (mEnableAntiAliasing)
	{
		mSMAA->Run(mD3D->GetImmediateContext(),
			mIntermediateTexture->GetShaderResourceView(),
			mD3D->GetDepthStencilSRView(),
			mDeferredBuffers->GetSRV(DeferredBuffersIndex::Velocity),
			mD3D->GetRenderTargetView(),
			mD3D->GetDepthStencilView());
	}

	//-------------------------------------------------------------------------------------
	// Motion blur cache
	//-------------------------------------------------------------------------------------
	// Store current view projection matrix as previous view proj for use in next frame
	//XMStoreFloat4x4(&mPrevViewProjMatrix, mCamera->GetViewProjMatrix());
	mCamera->SetPrevViewProj(mCamera->GetViewProjMatrix());

	// Store world matrix for use in next frame
	for (UINT i = 0; i < mInstances.size(); ++i)
	{
		if (mInstances[i]->IsVisible())
		{
			mInstances[i]->SetPrevWorld(mInstances[i]->GetWorld());
		}
	}

	for (UINT i = 0; i < mAnimatedInstances.size(); ++i)
	{
		if (mAnimatedInstances[i]->IsVisible())
		{
			mAnimatedInstances[i]->SetPrevWorld(mAnimatedInstances[i]->GetWorld());
		}
	}

	for (UINT i = 0; i < mMorphInstances.size(); ++i)
	{
		if (mMorphInstances[i]->IsVisible())
		{
			mMorphInstances[i]->prevWorld = mMorphInstances[i]->world;
		}
	}


	for (UINT i = 0; i < mSkinnedSortedInstances.size(); ++i)
	{
		if (mSkinnedSortedInstances[i]->isVisible)
		{
			mSkinnedSortedInstances[i]->prevWorld = mSkinnedSortedInstances[i]->world;
		}
	}

	renderTarget = mD3D->GetRenderTargetView();
	mD3D->GetImmediateContext()->OMSetRenderTargets(1, &renderTarget, NULL);
	mD3D->GetImmediateContext()->OMSetBlendState(0, blendFactor, 0xffffffff);

	//Debugging shadowcascades
	//mSpriteBatch->Begin();

	//mSpriteBatch->Draw(
	//	mCascadedShadows->GetCascade(0)->getDepthMapSRV(),
	//	XMFLOAT2(225.0f, 0.0f),
	//	nullptr,
	//	Colors::Red,
	//	0.0f,
	//	XMFLOAT2(0.0f, 0.0f),
	//	XMFLOAT2(0.1f, 0.1f)
	//	);

	//mSpriteBatch->Draw(
	//	mCascadedShadows->GetCascade(1)->getDepthMapSRV(),
	//	XMFLOAT2(450.0f, 0.0f),
	//	nullptr,
	//	Colors::Red,
	//	0.0f,
	//	XMFLOAT2(0.0f, 0.0f),
	//	XMFLOAT2(0.1f, 0.1f)
	//	);

	//mSpriteBatch->Draw(
	//	mCascadedShadows->GetCascade(2)->getDepthMapSRV(),
	//	XMFLOAT2(675.0f, 0.0f),
	//	nullptr,
	//	Colors::Red,
	//	0.0f,
	//	XMFLOAT2(0.0f, 0.0f),
	//	XMFLOAT2(0.1f, 0.1f)
	//	);

	//mSpriteBatch->End();

	/*
	//XMFLOAT4 test = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMVECTORF32 Green = { 0.000000000f, 1.000000000f, 0.000000000f, 1.000000000f };
	mSpriteBatch->Begin();
	// 	mSpriteBatch->Draw(mD3D->GetDepthStencilSRView(), XMFLOAT2(0.0f, 0.0f), nullptr, Colors::White, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.25f, 0.25f));
	// 	mSpriteBatch->Draw(mDeferredBuffers->GetLitSceneSRV(), XMFLOAT2(0.0f, 400.0f), nullptr, Colors::White, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.25f, 0.25f));
	// 	mSpriteBatch->Draw(mD3D->GetDepthStencilSRView(), XMFLOAT2(0.0f, 200.0f), nullptr, Green, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.25f, 0.25f));
	mSpriteBatch->Draw(mSMAA->GetSRV(SmaaBufferIndex::Edges), XMFLOAT2(0.0f, 0.0f), nullptr, Colors::White, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));
	mSpriteBatch->Draw(mSMAA->GetSRV(SmaaBufferIndex::Blend), XMFLOAT2(0.0f, 200.0f), nullptr, Colors::White, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.25f, 0.25f));
	mSpriteBatch->Draw(mDepthStencilSRVCopy, XMFLOAT2(0.0f, 400.0f), nullptr, Colors::White, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.25f, 0.25f));
	mSpriteBatch->End();
	*/

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

	mD3D->GetImmediateContext()->OMSetRenderTargets(0, nullptr, nullptr);
}

void GraphicsEngineImpl::UpdateScene(float dt, float gameTime)
{
	mGameTime = gameTime;
	mCurFPS = 1000.0f / dt;
	mCurFPS = mCurFPS / 1000.0f;

	// Update skinned instances
	for (size_t i = 0; i < mAnimatedInstances.size(); i++)
	{
		mAnimatedInstances[i]->Update(dt);
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
		//mParticleSystems[i]->SetParticleType((ParticleType)((rand() % ParticleType::NROFTYPES - 1) + 1));
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
		//mTextureMgr->CreateLevelTexture(file),
		mTextureMgr->CreateStaticTexture(file),
		input->pos,
		0,
		input->color,
		input->rot,
		input->origin,
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
		Colors::White,
		0.0f,
		XMFLOAT2(0.0f, 0.0f),
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
	if (mSkinnedModels.find(file) == mSkinnedModels.end())
	{
		std::stringstream ss;
		ss << file << ".dae";
		mSkinnedModels[file] = new GenericSkinnedModel(mD3D->GetDevice(),
			mTextureMgr,
			mResourceDir + ss.str());
	}

	AnimatedInstanceImpl* mi = new AnimatedInstanceImpl(Vec3(), Vec3(), Vec3(1, 1, 1), mSkinnedModels[file]);
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
		else if (mi->mSkinnedInstance->model == mAnimatedInstances[i]->mSkinnedInstance->model)
			found = true;
	}

	if (index != -1)
		mAnimatedInstances.erase(mAnimatedInstances.begin() + index);

	if (!found) //delete model if no other instance uses it
	{
		for (std::map<std::string, GenericSkinnedModel*>::iterator it = mSkinnedModels.begin(); it != mSkinnedModels.end(); it++)
		{
			if (it->second == mi->mSkinnedInstance->model)
			{
				mSkinnedModels.erase(it);
				break;
			}
		}
		delete mi->mSkinnedInstance->model;
	}

	delete mi;
}

MorphModelInstance* GraphicsEngineImpl::CreateMorphAnimatedInstance(std::string path, std::string file, Vec3 pos)
{
	MorphModel *model = new MorphModel(mD3D->GetDevice(), mTextureMgr, mResourceDir + path, file);
	MorphModelInstanceImpl *m = new MorphModelInstanceImpl();

	m->model = model;
	m->Set(pos, Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(0.0f, 0.0f, 0.0f));

	mMorphModels.push_back(model);
	mMorphInstances.push_back(m);

	return (m);
}

void GraphicsEngineImpl::DeleteInstance(MorphModelInstance* mmi)
{
	MorphModelInstanceImpl* m = (MorphModelInstanceImpl*)mmi;
	bool found = false;
	int index = -1;
	for (unsigned int i = 0; i < mMorphInstances.size(); i++)
	{
		if (m == mMorphInstances[i])
			index = i;
		else if (m->model == mMorphInstances[i]->model)
			found = true;
	}

	if (index != -1)
		mMorphInstances.erase(mMorphInstances.begin() + index);

	if (!found) //delete model if no other instance uses it
	{
		for (std::vector<MorphModel*>::iterator it = mMorphModels.begin(); it != mMorphModels.end(); it++)
		{
			if (*it == m->model)
			{
				mMorphModels.erase(it);
				break;
			}
		}
		delete m->model;
	}

	delete m;
}

Texture2D *GraphicsEngineImpl::CreateTexture2D(unsigned int width, unsigned int height, bool renderable)
{
	Texture2DImpl *texture = new Texture2DImpl(mD3D->GetDevice(), mD3D->GetImmediateContext(), width, height, DXGI_FORMAT_R8G8B8A8_UNORM, renderable);

	return (Texture2D *)texture;
}

void GraphicsEngineImpl::DeleteTexture2D(Texture2D *texture)
{
	Texture2DImpl *textureImpl = (Texture2DImpl *)texture;

	delete textureImpl;
}

ParticleSystem *GraphicsEngineImpl::CreateParticleSystem()
{
	ParticleSystemImpl* particleSystem = new ParticleSystemImpl();
	particleSystem->Init(mD3D->GetDevice(),
		mShaderHandler->mParticleSystemShader,
		mParticlesTextureArray,
		mRandom1DTexSRV,
		1000);

	// Set some default values.
	particleSystem->SetEmitPos(XMFLOAT3(0.0f, 15.0f, 0.0f));
	particleSystem->SetConstantAccel(XMFLOAT3(0.0f, 7.8f, 0.0f));
	particleSystem->SetParticleType(ParticleType::PT_FLARE1);

	mParticleSystems.push_back(particleSystem);

	return (ParticleSystem *)particleSystem;
}

void GraphicsEngineImpl::DeleteParticleSystem(ParticleSystem *particleSystem)
{
	ParticleSystemImpl* particleSystemImpl = (ParticleSystemImpl *)particleSystem;

	for (auto iter = mParticleSystems.begin(); iter != mParticleSystems.end(); iter++)
	{
		if ((*iter) == particleSystemImpl)
		{
			mParticleSystems.erase(iter);
			break;
		}
	}

	delete particleSystemImpl;
}

void GraphicsEngineImpl::OnResize(UINT width, UINT height)
{
	mScreenWidth = width;
	mScreenHeight = height;

	mD3D->OnResize(width, height);
	ReleaseCOM(mDepthStencilTextureCopy);
	ReleaseCOM(mDepthStencilSRVCopy);
	mDeferredBuffers->OnResize(mD3D->GetDevice(), width, height);
	mCamera->SetLens(fovY, (float)width / height, zNear, zFar);
	mCamera->UpdateOrthoMatrix(static_cast<float>(width), static_cast<float>(height), zNear, zFar);
	mOrthoWindow->OnResize(mD3D->GetDevice(), width, height);

	mSMAA->OnResize(mD3D->GetDevice(), width, height);

	// Resize Post-processing textures
	mIntermediateTexture->Resize(mD3D->GetDevice(), width, height);

	mSSAOTexture->Resize(mD3D->GetDevice(), (UINT)(width * mSSAOScale), (UINT)(height * mSSAOScale));
	mSSAOBlurTexture->Resize(mD3D->GetDevice(), (UINT)(width * mSSAOScale), (UINT)(height * mSSAOScale));

	mDoFCoCTexture->Resize(mD3D->GetDevice(), (UINT)(width * mDoFScale), (UINT)(height * mDoFScale));
	mDoFBlurTexture1->Resize(mD3D->GetDevice(), (UINT)(width * mDoFScale), (UINT)(height * mDoFScale));
	mDoFBlurTexture2->Resize(mD3D->GetDevice(), (UINT)(width * mDoFScale), (UINT)(height * mDoFScale));

	// Resize depth/stencil copy
	HRESULT hr;

	// Depth/stencil buffer copy
	// Create the depth/stencil buffer and view
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = width;						// Texture width in texels
	depthStencilDesc.Height = height;					// Texture height in texels
	depthStencilDesc.MipLevels = 1;								// Number of mipmap levels
	depthStencilDesc.ArraySize = 1;								// Number of textures in texture array
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;	// Texel format

	// Set number of multisamples and quality level for the depth/stencil buffer
	// This has to match swap chain MSAA values
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;			// How the texture will be used
	depthStencilDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	// Where the resource will be bound to the pipeline
	depthStencilDesc.CPUAccessFlags = 0;					// Specify CPU access (Only GPU writes/reads to the depth/buffer)
	depthStencilDesc.MiscFlags = 0;							// Optional flags

	hr = mD3D->GetDevice()->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilTextureCopy);

	D3D11_SHADER_RESOURCE_VIEW_DESC depthStencilSRViewDesc;
	memset(&depthStencilSRViewDesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	depthStencilSRViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	depthStencilSRViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	depthStencilSRViewDesc.Texture2D.MipLevels = 1;

	hr = mD3D->GetDevice()->CreateShaderResourceView(mDepthStencilTextureCopy, &depthStencilSRViewDesc, &mDepthStencilSRVCopy);
}

void GraphicsEngineImpl::RenderSceneToTexture()
{
	mDeferredBuffers->SetRenderTargets(mD3D->GetImmediateContext(), mD3D->GetDepthStencilView());
	mD3D->GetImmediateContext()->RSSetViewports(1, &mD3D->GetScreenViewport());

	mDeferredBuffers->ClearRenderTargets(mD3D->GetImmediateContext(), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), mD3D->GetDepthStencilView());
	mD3D->GetImmediateContext()->ClearRenderTargetView(mDeferredBuffers->GetLitSceneRTV(), reinterpret_cast<const float*>(&Colors::White));

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	mD3D->GetImmediateContext()->RSSetState(0);
	mD3D->GetImmediateContext()->OMSetDepthStencilState(0, 0);
	mD3D->GetImmediateContext()->OMSetBlendState(0, blendFactor, 0xffffffff);

	ID3D11RenderTargetView* renderTargetsLitScene[1] = { mDeferredBuffers->GetLitSceneRTV() };

	mD3D->GetImmediateContext()->OMSetBlendState(RenderStates::mDefaultBS, blendFactor, 0xffffffff);

	mD3D->GetImmediateContext()->RSSetState(RenderStates::mDefaultRS);

	//---------------------------------------------------------------------------------------
	// Sky
	//---------------------------------------------------------------------------------------
	mSky->Draw(mD3D->GetImmediateContext(), *mCamera, mShaderHandler->mSkyDeferredShader);

	// Enable stencil testing (subsequent draw calls will set stencil bits to 1)
	// Because the sky was drawn before setting this, the stencil bits that aren't set to 1 
	// (remained 0) will therefore mean that this is the sky.
	mD3D->GetImmediateContext()->OMSetDepthStencilState(RenderStates::mDepthStencilEnabledDSS, 1);

	// Sky shader uses no culling, so switch back to back face culling
	mD3D->GetImmediateContext()->RSSetState(RenderStates::mDefaultRS);

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

	//Set variables that are shared for all cascades
	mShaderHandler->mBasicDeferredShader->SetEyeSpaceTransform(this->mCamera->GetViewMatrix());
	mShaderHandler->mBasicDeferredShader->SetNrOfCascades(this->mCascadedShadows->GetNrOfCascades());

	Cascade* currCasc;

	//Loop through all cascades and set variables for the shader that renders model instances (shadowtransform, cascade texture, splitdepths)
	for (UINT cIndex = 0; cIndex < this->mCascadedShadows->GetNrOfCascades(); cIndex++)
	{
		currCasc = mCascadedShadows->GetCascade(cIndex);

		if (currCasc)
		{
			mShaderHandler->mBasicDeferredShader->SetCascadeTex(mD3D->GetImmediateContext(), currCasc->getDepthMapSRV(), cIndex);
			mShaderHandler->mBasicDeferredShader->SetCascadeTransformAndDepths(
				currCasc->GetShadowTransform(),
				currCasc->GetSplitDepthNear(),
				currCasc->GetSplitDepthFar(),
				cIndex);
		}
	}

	// Loop through all model instances
	for (UINT i = 0; i < mInstances.size(); ++i)
	{
		if (mInstances[i]->IsVisible())
		{
			mShaderHandler->mBasicDeferredShader->SetType(mInstances[i]->GetType());


			mShaderHandler->mBasicDeferredShader->SetWorldViewProjTex(mInstances[i]->GetWorld(),
				mCamera->GetViewProjMatrix(),
				toTexSpace);

			mShaderHandler->mBasicDeferredShader->SetPrevWorldViewProj(mInstances[i]->GetPrevWorld(),
				mCamera->GetPreviousViewProj());

			for (UINT j = 0; j < mInstances[i]->model->meshCount; ++j)
			{
				UINT matIndex = mInstances[i]->model->meshes[j].MaterialIndex;

				mShaderHandler->mBasicDeferredShader->SetMaterial(mInstances[i]->model->mat[matIndex],
					mInstances[i]->model->mGlobalMaterialIndex[matIndex]);
				mShaderHandler->mBasicDeferredShader->SetDiffuseMap(mD3D->GetImmediateContext(), mInstances[i]->model->diffuseMapSRV[matIndex].Get());
				mShaderHandler->mBasicDeferredShader->UpdatePerObj(mD3D->GetImmediateContext());
				mD3D->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				mInstances[i]->model->meshes[j].Draw(mD3D->GetImmediateContext());
			}
		}
	}

	for (UINT cIndex = 0; cIndex < MAX_CASC; cIndex++)
	{
		mShaderHandler->mBasicDeferredShader->SetCascadeTex(mD3D->GetImmediateContext(), NULL, cIndex);
	}

	//---------------------------------------------------------------------------------------
	// Skinned opaque objects
	//---------------------------------------------------------------------------------------
	mShaderHandler->mBasicDeferredSkinnedShader->SetActive(mD3D->GetImmediateContext());

	//Set variables that are shared for all cascades
	mShaderHandler->mBasicDeferredSkinnedShader->SetEyeSpaceTransform(this->mCamera->GetViewMatrix());
	mShaderHandler->mBasicDeferredSkinnedShader->SetNrOfCascades(this->mCascadedShadows->GetNrOfCascades());

	//Loop through all cascades and set variables for the shader that renders skinned instances (shadowtransform, cascade texture, splitdepths)
	for (UINT cIndex = 0; cIndex < this->mCascadedShadows->GetNrOfCascades(); cIndex++)
	{
		currCasc = mCascadedShadows->GetCascade(cIndex);

		if (currCasc)
		{
			mShaderHandler->mBasicDeferredSkinnedShader->SetCascadeTex(mD3D->GetImmediateContext(), currCasc->getDepthMapSRV(), cIndex);
			mShaderHandler->mBasicDeferredSkinnedShader->SetCascadeTransformAndDepths(
				currCasc->GetShadowTransform(),
				currCasc->GetSplitDepthNear(),
				currCasc->GetSplitDepthFar(),
				cIndex);
		}
	}

	// Loop through all model instances
	for (UINT i = 0; i < mAnimatedInstances.size(); ++i)
	{
		if (mAnimatedInstances[i]->IsVisible())
		{
			mShaderHandler->mBasicDeferredSkinnedShader->SetPrevWorldViewProj(mAnimatedInstances[i]->GetPrevWorld(), mCamera->GetPreviousViewProj());
			mAnimatedInstances[i]->Draw(mD3D->GetImmediateContext(), mCamera, mShaderHandler->mBasicDeferredSkinnedShader);
		}
	}

	for (UINT cIndex = 0; cIndex < MAX_CASC; cIndex++)
	{
		mShaderHandler->mBasicDeferredSkinnedShader->SetCascadeTex(mD3D->GetImmediateContext(), NULL, cIndex);
	}

	//---------------------------------------------------------------------------------------
	// Morphed opaque objects
	//---------------------------------------------------------------------------------------
	mShaderHandler->mDeferredMorphShader->SetActive(mD3D->GetImmediateContext());

	//Set variables that are shared for all cascades
	mShaderHandler->mDeferredMorphShader->SetEyeSpaceTransform(this->mCamera->GetViewMatrix());
	mShaderHandler->mDeferredMorphShader->SetNrOfCascades(this->mCascadedShadows->GetNrOfCascades());

	//Loop through all cascades and set variables for the shader that renders morph instances (shadowtransform, cascade texture, splitdepths)
	for (UINT cIndex = 0; cIndex < this->mCascadedShadows->GetNrOfCascades(); cIndex++)
	{
		currCasc = mCascadedShadows->GetCascade(cIndex);

		if (currCasc)
		{
			mShaderHandler->mDeferredMorphShader->SetCascadeTex(mD3D->GetImmediateContext(), currCasc->getDepthMapSRV(), cIndex);
			mShaderHandler->mDeferredMorphShader->SetCascadeTransformAndDepths(
				currCasc->GetShadowTransform(),
				currCasc->GetSplitDepthNear(),
				currCasc->GetSplitDepthFar(),
				cIndex);
		}
	}

	// No back face culling for morphed objects
	mD3D->GetImmediateContext()->RSSetState(RenderStates::mNoCullRS);
	for (UINT i = 0; i < mMorphInstances.size(); ++i)
	{
		if (mMorphInstances[i]->IsVisible())
		{

			mShaderHandler->mDeferredMorphShader->SetWorldViewProjTex(XMLoadFloat4x4(&mMorphInstances[i]->world),
				mCamera->GetViewProjMatrix(),
				toTexSpace);

			mShaderHandler->mDeferredMorphShader->SetPrevWorldViewProj(XMLoadFloat4x4(&mMorphInstances[i]->prevWorld), mCamera->GetPreviousViewProj());

			mShaderHandler->mDeferredMorphShader->SetWeights(mMorphInstances[i]->weights);
			mD3D->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			for (UINT j = 0; j < mMorphInstances[i]->model->mTargetModels.begin()->nrOfMeshes; ++j)
			{
				UINT matIndex = mMorphInstances[i]->model->mTargetModels.begin()->meshes[j].MaterialIndex;
				mShaderHandler->mDeferredMorphShader->SetMaterial(mMorphInstances[i]->model->mat[matIndex],
					mMorphInstances[i]->model->mGlobalMaterialIndex[matIndex]);
				mShaderHandler->mDeferredMorphShader->SetDiffuseMap(mD3D->GetImmediateContext(), mMorphInstances[i]->model->diffuseMapSRV[matIndex].Get());
				mShaderHandler->mDeferredMorphShader->UpdatePerObj(mD3D->GetImmediateContext());

				mMorphInstances[i]->model->Draw(mD3D->GetImmediateContext());
			}
		}
	}

	for (UINT cIndex = 0; cIndex < MAX_CASC; cIndex++)
	{
		mShaderHandler->mDeferredMorphShader->SetCascadeTex(mD3D->GetImmediateContext(), NULL, cIndex);
	}
	mD3D->GetImmediateContext()->RSSetState(RenderStates::mDefaultRS);

	//---------------------------------------------------------------------------------------
	// Skinned opaque objects with separated upper and lower body transformations
	//---------------------------------------------------------------------------------------

	//NOTE: CONFIRM THAT SHADOWING WORKS AS INTENDED FOR THIS SHADER, ONCE IT IS USED
	mShaderHandler->mBasicDeferredSkinnedSortedShader->SetActive(mD3D->GetImmediateContext());

	//Set variables that are shared for all cascades
	mShaderHandler->mBasicDeferredSkinnedSortedShader->SetEyeSpaceTransform(this->mCamera->GetViewMatrix());
	mShaderHandler->mBasicDeferredSkinnedSortedShader->SetNrOfCascades(this->mCascadedShadows->GetNrOfCascades());

	//Loop through all cascades and set variables for the shader that renders skinned instances with animation blending (shadowtransform, cascade texture, splitdepths)
	for (UINT cIndex = 0; cIndex < this->mCascadedShadows->GetNrOfCascades(); cIndex++)
	{
		currCasc = mCascadedShadows->GetCascade(cIndex);
		if (currCasc)
		{
			mShaderHandler->mBasicDeferredSkinnedSortedShader->SetCascadeTex(mD3D->GetImmediateContext(), currCasc->getDepthMapSRV(), cIndex);
			mShaderHandler->mBasicDeferredSkinnedSortedShader->SetCascadeTransform(currCasc->GetShadowTransform(), cIndex);
			mShaderHandler->mBasicDeferredSkinnedSortedShader->SetCascadeDepths(currCasc->GetSplitDepthNear(), currCasc->GetSplitDepthFar(), cIndex);
		}
	}

	for (UINT i = 0; i < mSkinnedSortedInstances.size(); ++i)
	{
		if (mSkinnedSortedInstances[i]->isVisible)
		{
			mShaderHandler->mBasicDeferredSkinnedSortedShader->SetWorldViewProjTex(XMLoadFloat4x4(&mSkinnedSortedInstances[i]->world), mCamera->GetViewProjMatrix(), toTexSpace);
			mShaderHandler->mBasicDeferredSkinnedSortedShader->SetPrevWorldViewProj(XMLoadFloat4x4(&mSkinnedSortedInstances[i]->prevWorld), mCamera->GetPreviousViewProj());

			mShaderHandler->mBasicDeferredSkinnedSortedShader->SetBoneTransforms(
				mSkinnedSortedInstances[i]->FinalLowerBodyTransforms.data(), (UINT)mSkinnedSortedInstances[i]->FinalLowerBodyTransforms.size(),
				mSkinnedSortedInstances[i]->FinalUpperBodyTransforms.data(), (UINT)mSkinnedSortedInstances[i]->FinalUpperBodyTransforms.size());

			mShaderHandler->mBasicDeferredSkinnedSortedShader->SetRootBoneIndex(mSkinnedSortedInstances[i]->model->skinnedData.RootBoneIndex);

			mD3D->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			for (UINT j = 0; j < mSkinnedSortedInstances[i]->model->meshes.size(); ++j)
			{
				UINT matIndex = mSkinnedSortedInstances[i]->model->meshes[j].mMaterialIndex;
				mShaderHandler->mBasicDeferredSkinnedSortedShader->SetMaterial(mSkinnedSortedInstances[i]->model->mat[matIndex],
					mSkinnedSortedInstances[i]->model->mGlobalMaterialIndex[matIndex]);
				mShaderHandler->mBasicDeferredSkinnedSortedShader->SetDiffuseMap(mD3D->GetImmediateContext(), mSkinnedSortedInstances[i]->model->diffuseMapSRV[matIndex].Get());
				mShaderHandler->mBasicDeferredSkinnedSortedShader->UpdatePerObj(mD3D->GetImmediateContext());

				mSkinnedSortedInstances[i]->model->meshes[j].draw(mD3D->GetImmediateContext());
			}
		}
	}

	for (UINT cIndex = 0; cIndex < MAX_CASC; cIndex++)
	{
		mShaderHandler->mBasicDeferredSkinnedSortedShader->SetCascadeTex(mD3D->GetImmediateContext(), NULL, cIndex);
	}

	// Set render target to light accumulation buffer, also use the depth/stencil buffer with previous stencil information
	mD3D->GetImmediateContext()->OMSetRenderTargets(1, renderTargetsLitScene, mD3D->GetDepthStencilView());
	mD3D->GetImmediateContext()->OMSetDepthStencilState(RenderStates::mDepthDisabledStencilUseDSS, 1); // Draw using stencil values of 1

	//---------------------------------------------------------------------------------------
	// Opaque objects lighting
	//---------------------------------------------------------------------------------------
	mShaderHandler->mLightDeferredToTextureShader->SetActive(mD3D->GetImmediateContext());
	mShaderHandler->mLightDeferredToTextureShader->SetEyePosW(mCamera->GetPosition());
	mShaderHandler->mLightDeferredToTextureShader->SetPLights(mD3D->GetImmediateContext(), (UINT)mPointLightsCount, mPointLights);
	mShaderHandler->mLightDeferredToTextureShader->SetDirLights(mD3D->GetImmediateContext(), (UINT)mDirLightsCount, mDirLights);
	mShaderHandler->mLightDeferredToTextureShader->SetSLights(mD3D->GetImmediateContext(), (UINT)mSpotLightsCount, mSpotLights);
	//mShaderHandler->mLightDeferredToTextureShader->SetShadowTransform(mShadowMap->GetShadowTransform());
	mShaderHandler->mLightDeferredToTextureShader->SetCameraViewProjMatrix(mCamera->GetViewMatrix(), mCamera->GetProjMatrix());
	//mShaderHandler->mLightDeferredToTextureShader->SetLightWorldViewProj(mShadowMap->GetLightWorld(), mShadowMap->GetLightView(), mShadowMap->GetLightProj());

	// TODO: Instead of hard coding these properties, get them from some modifiable settings collection
	mShaderHandler->mLightDeferredToTextureShader->SetFogProperties(1, 0.0195f, -125.0f, 0.105f, XMFLOAT4(0.86f, 0.86f, 0.9f, 1.0f));
	mShaderHandler->mLightDeferredToTextureShader->SetMotionBlurProperties(1);
	mShaderHandler->mLightDeferredToTextureShader->SetFpsValues(mCurFPS, mTargetFPS);
	mShaderHandler->mLightDeferredToTextureShader->SetSkipLighting(false);
	mShaderHandler->mLightDeferredToTextureShader->SetIsTransparencyPass(false);

	mShaderHandler->mLightDeferredToTextureShader->UpdatePerFrame(mD3D->GetImmediateContext());

	mShaderHandler->mLightDeferredToTextureShader->SetDiffuseTexture(mD3D->GetImmediateContext(), mDeferredBuffers->GetSRV(DeferredBuffersIndex::Diffuse));
	mShaderHandler->mLightDeferredToTextureShader->SetNormalTexture(mD3D->GetImmediateContext(), mDeferredBuffers->GetSRV(DeferredBuffersIndex::Normal));
	//mShaderHandler->mLightDeferredToTextureShader->SetSpecularTexture(mD3D->GetImmediateContext(), mDeferredBuffers->GetSRV(DeferredBuffersIndex::Specular));
	mShaderHandler->mLightDeferredToTextureShader->SetVelocityTexture(mD3D->GetImmediateContext(), mDeferredBuffers->GetSRV(DeferredBuffersIndex::Velocity));
	mShaderHandler->mLightDeferredToTextureShader->SetSSAOTexture(mD3D->GetImmediateContext(), mSSAOTexture->GetShaderResourceView());

	// I have to keep a copy of the depth/stencil buffer because the render target is set to use the buffer, while trying to send in the depth SRV to the shader.
	// That's why I send in a copy of the SRV instead.
	mD3D->GetImmediateContext()->CopyResource(mDepthStencilTextureCopy, mD3D->GetDepthStencilBuffer());
	mShaderHandler->mLightDeferredToTextureShader->SetDepthTexture(mD3D->GetImmediateContext(), mDepthStencilSRVCopy);

	mShaderHandler->mLightDeferredToTextureShader->SetWorldViewProj(XMMatrixIdentity(), mCamera->GetBaseViewMatrix(), mCamera->GetOrthoMatrix());
	mShaderHandler->mLightDeferredToTextureShader->UpdatePerObj(mD3D->GetImmediateContext());

	// Now render the window
	mOrthoWindow->Render(mD3D->GetImmediateContext());

	//---------------------------------------------------------------------------------------
	// Sky lighting
	//---------------------------------------------------------------------------------------
	mD3D->GetImmediateContext()->OMSetDepthStencilState(RenderStates::mDepthDisabledStencilUseDSS, 0); // Draw using stencil values of 0 (this is the sky)

	// We don't want any light to affect the sky, set these "num values" to 0
	mShaderHandler->mLightDeferredToTextureShader->SetPLights(mD3D->GetImmediateContext(), 0, mPointLights);
	mShaderHandler->mLightDeferredToTextureShader->SetDirLights(mD3D->GetImmediateContext(), 0, mDirLights);
	mShaderHandler->mLightDeferredToTextureShader->SetSLights(mD3D->GetImmediateContext(), 0, mSpotLights);
	mShaderHandler->mLightDeferredToTextureShader->SetSkipLighting(true);

	mShaderHandler->mLightDeferredToTextureShader->UpdatePerFrame(mD3D->GetImmediateContext());

	mOrthoWindow->Render(mD3D->GetImmediateContext());

	// Lastly, clear (unbind) the textures (otherwise D3D11 WARNING)
	mShaderHandler->mLightDeferredToTextureShader->SetDiffuseTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mLightDeferredToTextureShader->SetNormalTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mLightDeferredToTextureShader->SetSpecularTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mLightDeferredToTextureShader->SetSSAOTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mLightDeferredToTextureShader->SetDepthTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mLightDeferredToTextureShader->SetVelocityTexture(mD3D->GetImmediateContext(), NULL);

	// First layer of lit scene (opaque objects) should be done by now, begin drawing transparent objects

	//---------------------------------------------------------------------------------------
	// Transparent objects
	//---------------------------------------------------------------------------------------
	mDeferredBuffers->ClearRenderTargets(mD3D->GetImmediateContext(), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), mD3D->GetDepthStencilView());

	// Clear stencil
	mD3D->GetImmediateContext()->ClearDepthStencilView(mD3D->GetDepthStencilView(), D3D11_CLEAR_STENCIL, 1.0f, 0);

	mD3D->GetImmediateContext()->OMSetDepthStencilState(RenderStates::mDepthStencilEnabledDSS, 1);

	mDeferredBuffers->SetRenderTargets(mD3D->GetImmediateContext(), mD3D->GetDepthStencilView());

	// Set previously drawn lit scene as a texture to use in particle shader
	mShaderHandler->mParticleSystemShader->SetLitSceneTex(mD3D->GetImmediateContext(), mDeferredBuffers->GetLitSceneSRV());

	// I have to keep a copy of the depth/stencil buffer because the render target is set to use the buffer, while trying to send in the depth SRV to the shader.
	// That's why I send in a copy of the SRV instead.
	mD3D->GetImmediateContext()->CopyResource(mDepthStencilTextureCopy, mD3D->GetDepthStencilBuffer());
	mShaderHandler->mParticleSystemShader->SetDepthTexture(mD3D->GetImmediateContext(), mDepthStencilSRVCopy);

	for (UINT i = 0; i < mParticleSystems.size(); ++i)
	{
		if (mParticleSystems[i]->IsActive())
		{
			mParticleSystems[i]->SetEyePos(mCamera->GetPosition());
			mParticleSystems[i]->Draw(mD3D->GetImmediateContext(), *mCamera);
		}
	}

	// Unbind lit scene texture from particle shader
	mShaderHandler->mParticleSystemShader->SetLitSceneTex(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mParticleSystemShader->SetDepthTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mParticleSystemShader->ActivateDrawShaders(mD3D->GetImmediateContext());
	mShaderHandler->mParticleSystemShader->UpdateDrawShaders(mD3D->GetImmediateContext());

	// Clear diffuse from transparent objects with black
	//mD3D->GetImmediateContext()->ClearRenderTargetView(mDeferredBuffers->GetRenderTarget(DeferredBuffersIndex::Diffuse), reinterpret_cast<const float*>(&Colors::Black));

	// Restore to not using a geometry shader
	mD3D->GetImmediateContext()->GSSetShader(nullptr, nullptr, 0);

	//---------------------------------------------------------------------------------------
	// Transparent objects lighting
	//---------------------------------------------------------------------------------------
	mD3D->GetImmediateContext()->OMSetDepthStencilState(RenderStates::mDepthEnabledStencilUseDSS, 1);

	mD3D->GetImmediateContext()->OMSetRenderTargets(1, renderTargetsLitScene, mD3D->GetDepthStencilView());

	mD3D->GetImmediateContext()->OMSetBlendState(RenderStates::mDefaultBS, blendFactor, 0xffffffff);

	mShaderHandler->mLightDeferredToTextureShader->SetActive(mD3D->GetImmediateContext());
	mShaderHandler->mLightDeferredToTextureShader->UpdatePerFrame(mD3D->GetImmediateContext());
	mShaderHandler->mLightDeferredToTextureShader->UpdatePerObj(mD3D->GetImmediateContext());

	mShaderHandler->mLightDeferredToTextureShader->SetDiffuseTexture(mD3D->GetImmediateContext(), mDeferredBuffers->GetSRV(DeferredBuffersIndex::Diffuse));
	mShaderHandler->mLightDeferredToTextureShader->SetNormalTexture(mD3D->GetImmediateContext(), mDeferredBuffers->GetSRV(DeferredBuffersIndex::Normal));
	//mShaderHandler->mLightDeferredToTextureShader->SetSpecularTexture(mD3D->GetImmediateContext(), mDeferredBuffers->GetSRV(DeferredBuffersIndex::Specular));
	mShaderHandler->mLightDeferredToTextureShader->SetVelocityTexture(mD3D->GetImmediateContext(), mDeferredBuffers->GetSRV(DeferredBuffersIndex::Velocity));
	mShaderHandler->mLightDeferredToTextureShader->SetSSAOTexture(mD3D->GetImmediateContext(), mSSAOTexture->GetShaderResourceView());

	// I have to keep a copy of the depth/stencil buffer because the render target is set to use the buffer, while trying to send in the depth SRV to the shader.
	// That's why I send in a copy of the SRV instead.
	mD3D->GetImmediateContext()->CopyResource(mDepthStencilTextureCopy, mD3D->GetDepthStencilBuffer());
	mShaderHandler->mLightDeferredToTextureShader->SetDepthTexture(mD3D->GetImmediateContext(), mDepthStencilSRVCopy);

	mShaderHandler->mLightDeferredToTextureShader->SetBackgroundTexture(mD3D->GetImmediateContext(), mDeferredBuffers->GetSRV(DeferredBuffersIndex::Background));

	mShaderHandler->mLightDeferredToTextureShader->SetPLights(mD3D->GetImmediateContext(), (UINT)mPointLightsCount, mPointLights);
	mShaderHandler->mLightDeferredToTextureShader->SetDirLights(mD3D->GetImmediateContext(), (UINT)mDirLightsCount, mDirLights);
	mShaderHandler->mLightDeferredToTextureShader->SetSLights(mD3D->GetImmediateContext(), (UINT)mSpotLightsCount, mSpotLights);
	mShaderHandler->mLightDeferredToTextureShader->SetSkipLighting(false);
	mShaderHandler->mLightDeferredToTextureShader->SetIsTransparencyPass(true);

	mShaderHandler->mLightDeferredToTextureShader->UpdatePerFrame(mD3D->GetImmediateContext());

	// Now render the window
	mOrthoWindow->Render(mD3D->GetImmediateContext());

	mShaderHandler->mLightDeferredToTextureShader->SetDiffuseTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mLightDeferredToTextureShader->SetNormalTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mLightDeferredToTextureShader->SetSpecularTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mLightDeferredToTextureShader->SetSSAOTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mLightDeferredToTextureShader->SetDepthTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mLightDeferredToTextureShader->SetVelocityTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mLightDeferredToTextureShader->SetBackgroundTexture(mD3D->GetImmediateContext(), NULL);

	// Clear stencil buffer
	mD3D->GetImmediateContext()->ClearDepthStencilView(mD3D->GetDepthStencilView(), /*D3D11_CLEAR_DEPTH | */D3D11_CLEAR_STENCIL, 1.0f, 0);

	mD3D->GetImmediateContext()->RSSetState(0);
	mD3D->GetImmediateContext()->OMSetDepthStencilState(0, 0);
	mD3D->GetImmediateContext()->OMSetBlendState(0, blendFactor, 0xffffffff);

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

	//Used when creating orthogonal projection matrix for cascades
	BoundingBox::CreateFromPoints(this->mSceneBB, XMLoadFloat3(&minPt), XMLoadFloat3(&maxPt));

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

	//--------------------------------------------------------------------
	// Cache all the materials in this scene, by storing the addresses
	//--------------------------------------------------------------------
	// First clear the material cache
	mMaterials.clear();

	UINT curIndex = 0;

	// Cache static object materials
	for (auto& it(mModels.begin()); it != mModels.end(); ++it)
	{
		it->second->mGlobalMaterialIndex.clear();

		for (UINT i = 0; i < it->second->mat.size(); ++i)
		{

			// Find if an identical material has already been loaded
			int foundIndex = -1;

			for (UINT j = 0; j < mMaterials.size(); ++j)
			{
				if (it->second->mat[i] == *mMaterials[j])
				{
					foundIndex = j;
					break;
				}
			}

			if (foundIndex >= 0)
			{
				it->second->mGlobalMaterialIndex.push_back(foundIndex);
			}
			else
			{
				mMaterials.push_back(&it->second->mat[i]);
				it->second->mGlobalMaterialIndex.push_back(curIndex);
				curIndex++;
			}
		}
	}

	// Cache skinned object materials
	for (auto& it(mSkinnedModels.begin()); it != mSkinnedModels.end(); ++it)
	{
		it->second->mGlobalMaterialIndex.clear();

		for (UINT i = 0; i < it->second->mat.size(); ++i)
		{
			// Find if an identical material has already been loaded
			int foundIndex = -1;
			for (UINT j = 0; j < mMaterials.size(); ++j)
			{
				if (it->second->mat[i] == *mMaterials[j])
				{
					foundIndex = j;
					break;
				}
			}

			if (foundIndex >= 0)
			{
				it->second->mGlobalMaterialIndex.push_back(foundIndex);
			}
			else
			{
				mMaterials.push_back(&it->second->mat[i]);
				it->second->mGlobalMaterialIndex.push_back(curIndex);
				curIndex++;
			}
		}
		//}
	}

	// Cache morphed object materials
	// NOTE/FIXME: Right now textures and materials are loaded for each morph target
	// Even though the morph base model's materials and textures are the same as
	// the targets.
	// This takes up unnecessary space.
	// TODO: Load only materials and textures for the base model and use them.
	for (UINT i = 0; i < mMorphModels.size(); ++i)
	{
		mMorphModels[i]->mGlobalMaterialIndex.clear();

		for (UINT j = 0; j < mMorphModels[i]->mat.size(); ++j)
		{
			// Find if an identical material has already been loaded
			int foundIndex = -1;
			for (UINT k = 0; k < mMaterials.size(); ++k)
			{
				if (mMorphModels[i]->mat[j] == *mMaterials[k])
				{
					foundIndex = k;
					break;
				}
			}

			if (foundIndex >= 0)
			{
				mMorphModels[i]->mGlobalMaterialIndex.push_back(foundIndex);
			}
			else
			{
				mMaterials.push_back(&mMorphModels[i]->mat[j]);
				mMorphModels[i]->mGlobalMaterialIndex.push_back(curIndex);
				curIndex++;
			}
		}
		//}
	}

	// Cache skinned sorted object materials
	for (auto& it(mSkinnedSortedModels.begin()); it != mSkinnedSortedModels.end(); ++it)
	{
		it->second->mGlobalMaterialIndex.clear();

		for (UINT i = 0; i < it->second->mat.size(); ++i)
		{
			// Find if an identical material has already been loaded
			int foundIndex = -1;
			for (UINT j = 0; j < mMaterials.size(); ++j)
			{
				if (it->second->mat[i] == *mMaterials[j])
				{
					foundIndex = j;
					break;
				}
			}

			if (foundIndex >= 0)
			{
				it->second->mGlobalMaterialIndex.push_back(foundIndex);
			}
			else
			{
				mMaterials.push_back(&it->second->mat[i]);
				it->second->mGlobalMaterialIndex.push_back(curIndex);
				curIndex++;
			}
		}
	}

	mShaderHandler->mLightDeferredToTextureShader->SetMaterials(mD3D->GetImmediateContext(),
		mMaterials.size(), mMaterials.data());
}

DirectionalLight* GraphicsEngineImpl::AddDirLight(Vec3 color, Vec3 direction, float intensity)
{
	DirectionalLight* dl = new DirectionalLightImpl(&(mDirLights[mDirLightsCount]));
	mDirLightsCount++;

	color.X = pow(color.X, 2.2f);
	color.Y = pow(color.Y, 2.2f);
	color.Z = pow(color.Z, 2.2f);

	dl->SetAmbient(Vec3(intensity / 10.0f, intensity / 10.0f, intensity / 10.0f));
	dl->SetDiffuse(color);
	dl->SetDirection(direction);
	dl->SetSpecular(color);


	return dl;
}

PointLight* GraphicsEngineImpl::AddPointLight(Vec3 color, Vec3 position, float intensity)
{
	PointLight* pl = new PointLightImpl(&(mPointLights[mPointLightsCount]));
	mPointLightsCount++;

	color.X = pow(color.X, 2.2f);
	color.Y = pow(color.Y, 2.2f);
	color.Z = pow(color.Z, 2.2f);

	pl->SetPosition(position);
	pl->SetAmbient(color);
	pl->SetSpecular(color);
	pl->SetDiffuse(color);
	pl->SetAttenuation(Vec3(intensity / 100.0f, intensity / 100.0f, intensity / 100.0f));
	pl->SetRange(250.0f);


	return pl;
}

SpotLight* GraphicsEngineImpl::AddSpotLight(Vec3 color, Vec3 direction, Vec3 position, float angle)
{
	SpotLight* sl = new SpotLightImpl(&(mSpotLights[mSpotLightsCount]));
	mSpotLightsCount++;

	color.X = pow(color.X, 2.2f);
	color.Y = pow(color.Y, 2.2f);
	color.Z = pow(color.Z, 2.2f);

	sl->SetAmbient(color);
	sl->SetDiffuse(color);
	sl->SetSpecular(color);
	sl->SetAttenuation(Vec3(1.0f, 0.0f, 0.0f));
	sl->SetSpot(angle);
	sl->SetRange(500.0f);
	sl->SetPosition(position);
	sl->SetDirection(direction);


	return sl;
}

void GraphicsEngineImpl::ClearLights()
{
	mSpotLightsCount = 0;
	mDirLightsCount = 0;
	mPointLightsCount = 0;
}


void GraphicsEngineImpl::PrintText(std::string text, int x, int y, Vec3 color, float scale, float alpha)
{
	std::wstring t = std::wstring(text.begin(), text.end());
	XMVECTORF32 v_color = { color.X, color.Y, color.Z, alpha };
	mSpriteFont->DrawString(mSpriteBatch, t.c_str(), XMFLOAT2((float)x, (float)y), v_color, 0.0f, XMFLOAT2(0, 0), scale);
}

Vec3 GraphicsEngineImpl::MeassureString(const std::string text)
{
	std::wstring t = std::wstring(text.begin(), text.end());
	XMVECTOR size = mSpriteFont->MeasureString(t.c_str());

	return Vec3(size.m128_f32[0], size.m128_f32[1]);
}

void GraphicsEngineImpl::GetWindowResolution(UINT& width, UINT& height)
{
	width = mScreenWidth;
	height = mScreenHeight;
}
void GraphicsEngineImpl::Clear()
{
	mD3D->GetImmediateContext()->RSSetState(0);
	// Restore back and depth buffer and viewport to the OM stage
	ID3D11RenderTargetView* renderTargets[1] = { mD3D->GetRenderTargetView() };
	mD3D->GetImmediateContext()->OMSetRenderTargets(1, renderTargets, mD3D->GetDepthStencilView());
	mD3D->GetImmediateContext()->ClearRenderTargetView(mD3D->GetRenderTargetView(), reinterpret_cast<const float*>(&Colors::LightSteelBlue));

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

bool GraphicsEngineImpl::IsFullscreen()
{
	BOOL fullscreen;
	mD3D->GetSwapChain()->GetFullscreenState(&fullscreen, NULL);
	return fullscreen == 1;
}

void GraphicsEngineImpl::SetMorphAnimWeigth(unsigned index, Vec3 weight)
{
	if (mMorphInstances.size() > index)
	{
		mMorphInstances[index]->SetWeights(weight);
	}
}

Vec3 GraphicsEngineImpl::GetMorphAnimWeigth(unsigned index)
{
	if (mMorphInstances.size() > index)
	{
		return mMorphInstances[index]->GetWeights();
	}
	else
		return Vec3::Zero();
}

void GraphicsEngineImpl::ClearTexture(Texture2D *texture, const float color[4])
{
	assert(texture->IsRenderable());

	Texture2DImpl *textureImpl = (Texture2DImpl *)texture;
	mD3D->GetImmediateContext()->ClearRenderTargetView(textureImpl->GetRenderTargetView(), color);
}

void GraphicsEngineImpl::PrintTextMonospaceToTexture(Texture2D *texture, const std::string &text, const int position[2])
{
	assert(texture->IsRenderable());

	Texture2DImpl *textureImpl = (Texture2DImpl *)texture;
	ID3D11RenderTargetView *renderTarget = textureImpl->GetRenderTargetView();
	D3D11_VIEWPORT viewport;

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)texture->GetWidth();
	viewport.Height = (float)texture->GetHeight();
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	mD3D->GetImmediateContext()->OMSetRenderTargets(1, &renderTarget, NULL);
	mD3D->GetImmediateContext()->RSSetViewports(1, &viewport);

	std::wstring textWide(text.begin(), text.end());
	mSpriteFontMonospace->DrawString(mSpriteBatch, textWide.c_str(), XMFLOAT2((float)position[0], (float)position[1]));
}

void GraphicsEngineImpl::DrawLines(Texture2D *texture, const float *data, size_t count, const XMFLOAT3X3 &transformation, const float color[4])
{
	assert(texture->IsRenderable());

	if (count > mLineVertexBufferSize)
		printf("GraphicsEngineImpl::DrawLines, count is too damn high!\n");

	size_t clampedCount = std::min(count, mLineVertexBufferSize);

	// Update vertex buffer
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	mD3D->GetImmediateContext()->Map(mLineVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, data, sizeof(float)* 2 * clampedCount);
	mD3D->GetImmediateContext()->Unmap(mLineVertexBuffer, 0);

	mShaderHandler->mLineShader->SetTransformation(transformation);
	mShaderHandler->mLineShader->SetColor(XMFLOAT4(color[0], color[1], color[2], color[3]));
	mShaderHandler->mLineShader->SetFramebufferSize(XMFLOAT2((float)texture->GetWidth(), (float)texture->GetHeight()));

	mShaderHandler->mLineShader->Update(mD3D->GetImmediateContext());
	mShaderHandler->mLineShader->SetActive(mD3D->GetImmediateContext());

	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)texture->GetWidth();
	viewport.Height = (float)texture->GetHeight();
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	ID3D11RenderTargetView *renderTarget = ((Texture2DImpl *)texture)->GetRenderTargetView();
	UINT stride = sizeof(float)* 2;
	UINT offset = 0;

	mD3D->GetImmediateContext()->OMSetRenderTargets(1, &renderTarget, NULL);
	mD3D->GetImmediateContext()->RSSetViewports(1, &viewport);

	mD3D->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	mD3D->GetImmediateContext()->IASetVertexBuffers(0, 1, &mLineVertexBuffer, &stride, &offset);
	mD3D->GetImmediateContext()->Draw((UINT)clampedCount, 0);
}

void GraphicsEngineImpl::ResetRenderTargetAndViewport()
{
	ID3D11RenderTargetView *renderTarget = mD3D->GetRenderTargetView();
	mD3D->GetImmediateContext()->OMSetRenderTargets(1, &renderTarget, mD3D->GetDepthStencilView());
	mD3D->GetImmediateContext()->RSSetViewports(1, &mD3D->GetScreenViewport());
}

void GraphicsEngineImpl::ClearLevelTextures()
{
	//mTextureMgr->ClearLevel();

	for (auto& itTex(mTextureMgr->mLevelTextureSRV.begin()); itTex != mTextureMgr->mLevelTextureSRV.end();)
	{
		bool textureIsUsed = false;

		for (auto& it(mModels.begin()); it != mModels.end(); ++it)
		{
			for (UINT i = 0; i < it->second->diffuseMapSRV.size(); ++i)
			{
				if (it->second->diffuseMapSRV[i].Get() == itTex->second.Get())
				{
					textureIsUsed = true;
					break;
				}
			}
		}

		if (!textureIsUsed)
		{
			for (auto& it(mSkinnedModels.begin()); it != mSkinnedModels.end(); ++it)
			{
				for (UINT i = 0; i < it->second->diffuseMapSRV.size(); ++i)
				{
					if (it->second->diffuseMapSRV[i].Get() == itTex->second.Get())
					{
						textureIsUsed = true;
						break;
					}
				}
			}
		}

		if (!textureIsUsed)
		{
			for (UINT j = 0; j < mMorphModels.size(); ++j)
			{
				for (UINT i = 0; i < mMorphModels[j]->diffuseMapSRV.size(); ++i)
				{
					if (mMorphModels[j]->diffuseMapSRV[i].Get() == itTex->second.Get())
					{
						textureIsUsed = true;
						break;
					}
				}
			}
		}


		if (!textureIsUsed)
		{
			for (auto& it(mSkinnedSortedModels.begin()); it != mSkinnedSortedModels.end(); ++it)
			{
				for (UINT i = 0; i < it->second->diffuseMapSRV.size(); ++i)
				{
					if (it->second->diffuseMapSRV[i].Get() == itTex->second.Get())
					{
						textureIsUsed = true;
						break;
					}
				}
			}
		}

		if (!textureIsUsed)
		{
			itTex->second = nullptr;
			mTextureMgr->mLevelTextureSRV.erase(itTex++);
		}
		else
		{
			++itTex;
		}
	}
}

void GraphicsEngineImpl::SetSkyTexture(const std::string& fileName)
{
	mSky->SetTexture(mResourceDir + fileName, mTextureMgr);
}

void GraphicsEngineImpl::ClearModelInstances()
{
	for (UINT i = 0; i < mInstances.size(); ++i)
		DeleteInstance(mInstances[i]);

	for (UINT i = 0; i < mAnimatedInstances.size(); ++i)
		DeleteInstance(mAnimatedInstances[i]);

	for (UINT i = 0; i < mMorphInstances.size(); ++i)
		DeleteInstance(mMorphInstances[i]);

	for (UINT i = 0; i < mSkinnedSortedInstances.size(); ++i)
		delete mSkinnedSortedInstances[i];
}
