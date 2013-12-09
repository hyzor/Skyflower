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

	for (auto& it(mSkinnedModels.begin()); it != mSkinnedModels.end(); ++it)
	{
		if (it->second)
			SafeDelete(it->second);
	}
	mSkinnedModels.clear();

	//delete mD3dWindow;

	delete mCamera;

	mEffects->DestroyAll();
	delete mEffects;

	InputLayouts::DestroyAll();

	delete mAnimatedEntity;

	delete mTextureMgr;

	delete mCharacter;

	if (mDirect3D)
		delete mDirect3D;
}

bool GraphicsEngineImpl::Init(HWND hWindow)
{
	//mD3dWindow = new D3dWindow(hWindow);

	//if (!mD3dWindow)
	//	return false;

	//if (!mD3dWindow->Init())
	//	return false;

	int width = 1024;
	int height = 768;

	mDirect3D = new Direct3D();

	mDirect3D->Init(&hWindow, width, height);

	mEffects = new Effects();

	mEffects->InitAll(mDirect3D->GetDevice());
	InputLayouts::InitAll(mDirect3D->GetDevice());

	// Shaders
	//Effects::InitAll(mDirect3D->GetDevice());

	mTextureMgr = new TextureManager();

	// Texture manager
	mTextureMgr->Init(mDirect3D->GetDevice());


	// Load models
	mSkinnedModels["Character"] = new GenericSkinnedModel(mDirect3D->GetDevice(),
		mTextureMgr,
		"Data\\Models\\Character\\PlaceHolderPlayerexportsettingsTest.dae",
		L"Data\\Models\\Character\\");

	// -----------------------------------------
	// Test model instances
	// -----------------------------------------
	mCharacter = new Character(mSkinnedModels["Character"], XMFLOAT3(-90.0f, 0.0f, -300.0f));


	// Create camera
	mCamera = new Camera();

	mCamera->SetLens(0.25f*MathHelper::pi, static_cast<float>(width)/height, 1.0f, 10000.0f);
	mCamera->SetPosition(XMFLOAT3(0.0f, 50.0f, -500.0f));

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

	//mD3dWindow->ShowWindow();
	//mD3dWindow->OnResize();
	mDirect3D->OnResize();

	return true;
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

//int GraphicsEngineImpl::DrawScene()
//{
//	if (msg.message == WM_QUIT)
//		return 0;
//
//	// If there are Window messages, process them
//	if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
//	{
//		::TranslateMessage(&msg);
//		::DispatchMessage(&msg);
//		//std::cout << "Message!\n";
//	}
//	// Else run the game loop
//	else
//	{
//		//mD3dWindow->GetTimer()->tick();
//
//		//if (!mD3dWindow->AppPaused())
//		{
//			//std::cout << "Looping!\n";
//			//mD3dWindow->CalculateFrameStats();
//			UpdateScene(0.01f);//mD3dWindow->GetTimer()->getDeltaTime());
//			drawScene();
//			//mDirectInput->Update();
//			//UpdateScene(mTimer.getDeltaTime());
//			//DrawScene();
//		}
//		//else
//		{
//			//Sleep(100);
//		}
//	}
//
//	return 1;
//}

void GraphicsEngineImpl::DrawScene()
{
	//std::cout << "Drawing scene!\n";

	mDirect3D->GetImmediateContext()->RSSetState(0);
	// Restore back and depth buffer and viewport to the OM stage
	ID3D11RenderTargetView* renderTargets[1] = { mDirect3D->GetRenderTargetView() };
	mDirect3D->GetImmediateContext()->OMSetRenderTargets(1, renderTargets, mDirect3D->GetDepthStencilView());
	mDirect3D->GetImmediateContext()->ClearRenderTargetView(mDirect3D->GetRenderTargetView(), reinterpret_cast<const float*>(&Colors::LightSteelBlue));

	mDirect3D->GetImmediateContext()->ClearDepthStencilView(mDirect3D->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	mDirect3D->GetImmediateContext()->RSSetViewports(1, &mDirect3D->GetScreenViewport());

	// Set shader values
	mEffects->BasicFX->SetEyePosW(mCamera->GetPosition());
	mEffects->BasicFX->SetDirLights(mDirLights);

	// ---------------------------------------------------------------------
	// Specific entity draw function begin
	// ---------------------------------------------------------------------
	mDirect3D->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mDirect3D->GetImmediateContext()->IASetInputLayout(InputLayouts::Basic32);

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

		XMMATRIX modelScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
		XMMATRIX modelRot = XMMatrixRotationX(MathHelper::DegreesToRadians(-90.0f));
		modelRot *= XMMatrixRotationY(MathHelper::DegreesToRadians(22.5f));
		XMMATRIX modelOffset = XMMatrixTranslation(mInstances[i]->pos.X, mInstances[i]->pos.Y, mInstances[i]->pos.Z);
		//XMMATRIX world = modelScale*modelRot*modelOffset;
		world = modelScale*modelRot*modelOffset;

		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		mEffects->BasicFX->SetWorld(world);
		mEffects->BasicFX->SetWorldInvTranspose(worldInvTranspose);
		mEffects->BasicFX->SetWorldViewProj(worldViewProj);
		mEffects->BasicFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
		mEffects->BasicFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));


		GenericModel* model = mModels[mInstances[i]->model];

		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			for (UINT j = 0; j < model->meshCount; ++j)
			{
				UINT matIndex = model->meshes[j].MaterialIndex;

				mEffects->BasicFX->SetMaterial(model->mat[matIndex]);
				mEffects->BasicFX->SetDiffuseMap(model->diffuseMapSRV[matIndex]);

				activeTech->GetPassByIndex(p)->Apply(0, mDirect3D->GetImmediateContext());
				model->meshes[j].Draw(mDirect3D->GetImmediateContext());
			}
		}
	}

	// ---------------------------------------------------------------------
	// Specific entity draw function end
	// ---------------------------------------------------------------------
	mEffects->NormalMapFX->SetEyePosW(mCamera->GetPosition());
	mEffects->NormalMapFX->SetDirLights(mDirLights);

	mCharacter->Draw(mDirect3D->GetImmediateContext(), mEffects->NormalMapFX->DirLights3TexSkinnedTech, mCamera);

	// Restore default states
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	mDirect3D->GetImmediateContext()->RSSetState(0);
	mDirect3D->GetImmediateContext()->OMSetDepthStencilState(0, 0);
	mDirect3D->GetImmediateContext()->OMSetBlendState(0, blendFactor, 0xffffffff);

	HR(mDirect3D->GetSwapChain()->Present(0, 0));
}

void GraphicsEngineImpl::UpdateScene(float dt)
{
	//std::cout << "Updating scene!\n";

	mCamera->UpdateViewMatrix();

	mCharacter->Update(dt);
}

ModelInstance* GraphicsEngineImpl::CreateInstance(std::string file)
{
	if (mModels.find(file) == mModels.end())
	{
		mModels[file] = new GenericModel(mDirect3D->GetDevice(),
			mTextureMgr,
			file,
			L"Data\\Models\\");
	}

	ModelInstance* mi = new ModelInstance();
	mi->model = file;
	mi->isVisible = true;
	mi->pos = Vec3();
	mi->rot = Vec3();
	mi->scale = Vec3();

	mInstances.push_back(mi);
	return mi;
}

void GraphicsEngineImpl::DeleteInstance(ModelInstance* mi)
{

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
		GenericModel* model = mModels[mi->model];
		for (std::map<std::string, GenericModel*>::iterator it = mModels.begin(); it != mModels.end(); it++)
		{
			if (it->second == model)
			{
				mModels.erase(it);
				break;
			}
		}
		delete model;
	}

	delete mi;
}
