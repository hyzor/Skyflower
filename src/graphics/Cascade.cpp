#include "Cascade.h"

// Must be included last!
#include "shared/debug.h"

Cascade::Cascade(ID3D11Device* device, UINT width, UINT height)
: mDepthMapDSV(0), mDepthMapSRV(0)
{
	CreateCascade(device, width, height);
}

Cascade::Cascade(const Cascade& other)
: mDepthMapDSV(0), mDepthMapSRV(0)
{
	this->mHeight = other.mHeight;
	this->mWidth = other.mWidth;
	this->mLightProj = other.mLightProj;
	this->mLightView = other.mLightView;
	this->mLightWorld = other.mLightWorld;
	this->mShadowTransform = other.mShadowTransform;
	this->mViewport = other.mViewport;
}

Cascade::~Cascade(void)
{
	ReleaseCOM(mDepthMapSRV);
	ReleaseCOM(mDepthMapDSV);
// 	mRenderTargetTexture->Release();
// 	mRenderTargetView->Release();
}

ID3D11ShaderResourceView* Cascade::getDepthMapSRV()
{
	return mDepthMapSRV;
}

void Cascade::BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc)
{
	dc->RSSetViewports(1, &mViewport);

	// We set null render target because we only draw to depth buffer.
	// This will disable color writes
	ID3D11RenderTargetView* renderTargets[1] = { 0 };
	dc->OMSetRenderTargets(1, renderTargets, mDepthMapDSV);

	dc->ClearDepthStencilView(mDepthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Cascade::SetResolution(ID3D11Device* device, UINT width, UINT height)
{
	// Release old SRV and DSV
	ReleaseCOM(mDepthMapSRV);
	ReleaseCOM(mDepthMapDSV);

	CreateCascade(device, width, height);
}

void Cascade::SetSplitDepthNear(float zNear)
{
	this->zNearSplit = zNear;
}

void Cascade::SetSplitDepthFar(float zFar)
{
	this->zFarSplit = zFar;
}

bool Cascade::CreateCascade(ID3D11Device* device, UINT width, UINT height)
{
	// Set new dimensions
	mWidth = width;
	mHeight = height;

	// Specify viewport to match shadow map dimensions
	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	mViewport.Width = static_cast<float>(mWidth);
	mViewport.Height = static_cast<float>(mHeight);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	// Texture description
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	// Texture format is typeless because the DSV and SRV use different formats
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	// Now create the texture
	ID3D11Texture2D* depthMap = 0;
	HRESULT res = HR(device->CreateTexture2D(&texDesc, 0, &depthMap));

	// DSV description
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	// Create DSV
	res = HR(device->CreateDepthStencilView(depthMap, &dsvDesc, &mDepthMapDSV));

	// SRV description
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	// Create SRV
	res = HR(device->CreateShaderResourceView(depthMap, &srvDesc, &mDepthMapSRV));

	// Now release depth map reference because view already saved a reference
	ReleaseCOM(depthMap);

	return true;
}

UINT Cascade::GetWidth() const
{
	return mWidth;
}

UINT Cascade::GetHeight() const
{
	return mHeight;
}

float Cascade::GetSplitDepthNear() const
{
	return this->zNearSplit;
}

float Cascade::GetSplitDepthFar() const
{
	return this->zFarSplit;
}

void Cascade::SetShadowMatrices(const XMMATRIX& lightView, const XMMATRIX& proj, XMVECTOR lightPos)
{
	XMMATRIX V = lightView;
	XMMATRIX P = proj;

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX offset = XMMatrixTranslationFromVector(lightPos);
	XMMATRIX mrot = XMMatrixRotationX(0.0f);
	mrot *= XMMatrixRotationY(0.0f);
	mrot *= XMMatrixRotationZ(0.0f);
	XMMATRIX mscale = XMMatrixScaling(1.0f, 1.0f, 1.0f);

	XMMATRIX world = mscale*mrot*offset;

	XMStoreFloat4x4(&this->mLightWorld, world);
	XMStoreFloat4x4(&this->mLightView, lightView);
	XMStoreFloat4x4(&this->mLightProj, proj * T);
	XMStoreFloat4x4(&this->mShadowTransform, lightView * proj * T );
}

void Cascade::DrawSceneToShadowMap(
	const std::vector<ModelInstanceImpl*>& modelInstances,
	const std::vector<AnimatedInstanceImpl*>& mAnimatedInstances,
	ID3D11DeviceContext* deviceContext,
	ShadowShader* shadowShader,
	SkinnedShadowShader* sShadowShader)
{
	//deviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthMapDSV);

	XMMATRIX view = XMLoadFloat4x4(&mLightView);
	XMMATRIX proj = XMLoadFloat4x4(&mLightProj);
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shadowShader->SetActive(deviceContext);

	for (UINT i = 0; i < modelInstances.size(); ++i)
	{
		if (modelInstances[i]->IsVisible())
		{
			world = modelInstances[i]->GetWorld();
			worldInvTranspose = MathHelper::InverseTranspose(world);
				
			worldViewProj = world*viewProj;
			shadowShader->setLightWVP(deviceContext, worldViewProj);
			shadowShader->updatePerObj(deviceContext);


			for (UINT j = 0; j < modelInstances[i]->model->meshCount; ++j)
			{
				modelInstances[i]->model->meshes[j].Draw(deviceContext);
			}
		}
	}


	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	sShadowShader->SetActive(deviceContext);

	for (UINT i = 0; i < mAnimatedInstances.size(); ++i)
	{
		if (mAnimatedInstances[i]->IsVisible())
		{
			world = mAnimatedInstances[i]->GetWorld();
			worldViewProj = world * viewProj;

			sShadowShader->SetLightWVP(deviceContext, worldViewProj);
			sShadowShader->SetBoneTransforms(deviceContext, 
				mAnimatedInstances[i]->model->mInstance.FinalTransforms.data(), 
				(UINT)mAnimatedInstances[i]->model->mInstance.FinalTransforms.size());

			for (UINT j = 0; j < mAnimatedInstances[i]->model->mInstance.model->numMeshes; ++j)
			{
				sShadowShader->UpdatePerObj(deviceContext);
				mAnimatedInstances[i]->model->mInstance.model->meshes[j].draw(deviceContext);
			}
		}
	}

	deviceContext->RSSetState(0);
}

void Cascade::DrawSceneToShadowMap(const std::vector<ModelInstanceImpl*>& modelInstances,
	const std::vector<AnimatedInstanceImpl*>& mAnimatedInstances,
	const std::vector<MorphModelInstance*>& mMorphInstances,
	ID3D11DeviceContext* deviceContext,
	ShadowShader* shadowShader,
	SkinnedShadowShader* skinnedShadowShader,
	ShadowMorphShader* shadowMorphShader)
{
	XMMATRIX view = XMLoadFloat4x4(&mLightView);
	XMMATRIX proj = XMLoadFloat4x4(&mLightProj);
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shadowShader->SetActive(deviceContext);

	for (UINT i = 0; i < modelInstances.size(); ++i)
	{
		if (modelInstances[i]->IsVisible())
		{
			world = modelInstances[i]->GetWorld();
			worldInvTranspose = MathHelper::InverseTranspose(world);

			worldViewProj = world*viewProj;
			shadowShader->setLightWVP(deviceContext, worldViewProj);
			shadowShader->updatePerObj(deviceContext);


			for (UINT j = 0; j < modelInstances[i]->model->meshCount; ++j)
			{
				modelInstances[i]->model->meshes[j].Draw(deviceContext);
			}
		}
	}


	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	skinnedShadowShader->SetActive(deviceContext);

	for (UINT i = 0; i < mAnimatedInstances.size(); ++i)
	{
		if (mAnimatedInstances[i]->IsVisible())
		{
			world = mAnimatedInstances[i]->GetWorld();
			worldViewProj = world * viewProj;

			skinnedShadowShader->SetLightWVP(deviceContext, worldViewProj);
			skinnedShadowShader->SetBoneTransforms(deviceContext,
				mAnimatedInstances[i]->model->mInstance.FinalTransforms.data(),
				(UINT)mAnimatedInstances[i]->model->mInstance.FinalTransforms.size());

			for (UINT j = 0; j < mAnimatedInstances[i]->model->mInstance.model->numMeshes; ++j)
			{
				skinnedShadowShader->UpdatePerObj(deviceContext);
				mAnimatedInstances[i]->model->mInstance.model->meshes[j].draw(deviceContext);
			}
		}
	}

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	shadowMorphShader->SetActive(deviceContext);

	for (UINT i = 0; i < mMorphInstances.size(); ++i)
	{
		if (mMorphInstances[i]->isVisible)
		{
			world = XMLoadFloat4x4(&mMorphInstances[i]->world);
			worldViewProj = world * viewProj;

			shadowMorphShader->SetLightWVP(deviceContext, worldViewProj);
			shadowMorphShader->SetWeights(mMorphInstances[i]->weights);

			for (UINT j = 0; j < mMorphInstances[i]->model->mTargetModels.begin()->nrOfMeshes; ++j)
			{
				shadowMorphShader->UpdatePerObj(deviceContext);
				mMorphInstances[i]->model->Draw(deviceContext);
			}
		}
	}

	deviceContext->RSSetState(0);
}

XMMATRIX Cascade::GetShadowTransform() const
{
	return XMLoadFloat4x4(&mShadowTransform);
}

XMMATRIX Cascade::GetLightViewProj() const
{
	return XMMatrixMultiply(XMLoadFloat4x4(&mLightView), XMLoadFloat4x4(&mLightProj));
}

ID3D11DepthStencilView* Cascade::getDepthMapDSV()
{
	return mDepthMapDSV;
}

DirectX::XMMATRIX Cascade::GetLightWorld() const
{
	return XMLoadFloat4x4(&mLightWorld);
}

DirectX::XMMATRIX Cascade::GetLightView() const
{
	return XMLoadFloat4x4(&mLightView);
}

DirectX::XMMATRIX Cascade::GetLightProj() const
{
	return XMLoadFloat4x4(&mLightProj);
}
