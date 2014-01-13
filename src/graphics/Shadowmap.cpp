//------------------------------------------------------------------------------------
// File: ShadowMap.cpp
//
// This class encapsulates a depth/stencil buffer, views and viewport for
// shadow mapping
//------------------------------------------------------------------------------------
#include "ShadowMap.h"


ShadowMap::ShadowMap(ID3D11Device* device, UINT width, UINT height)
: mDepthMapDSV(0), mDepthMapSRV(0)
{
	CreateShadowMap(device, width, height);
}


ShadowMap::~ShadowMap(void)
{
	ReleaseCOM(mDepthMapSRV);
	ReleaseCOM(mDepthMapDSV);
}

ID3D11ShaderResourceView* ShadowMap::getDepthMapSRV()
{
	return mDepthMapSRV;
}

void ShadowMap::BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc)
{
	dc->RSSetViewports(1, &mViewport);

	// We set null render target because we only draw to depth buffer.
	// This will disable color writes
	ID3D11RenderTargetView* renderTargets[1] = { 0 };
	dc->OMSetRenderTargets(1, renderTargets, mDepthMapDSV);

	dc->ClearDepthStencilView(mDepthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void ShadowMap::SetResolution(ID3D11Device* device, UINT width, UINT height)
{
	// Release old SRV and DSV
	ReleaseCOM(mDepthMapSRV);
	ReleaseCOM(mDepthMapDSV);

	CreateShadowMap(device, width, height);
}

void ShadowMap::CreateShadowMap(ID3D11Device* device, UINT width, UINT height)
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
}

UINT ShadowMap::GetWidth() const
{
	return mWidth;
}

UINT ShadowMap::GetHeight() const
{
	return mHeight;
}

void ShadowMap::BuildShadowTransform(const DirectionalLight& light, const DirectX::BoundingSphere& sceneBounds)
{
	// Only first "main" light casts a shadow
	// So get light direction and position from first light
	XMVECTOR lightDir = XMLoadFloat3(&light.Direction);
	XMVECTOR lightPos = -2.0f*sceneBounds.Radius*lightDir;
	XMVECTOR targetPos = XMLoadFloat3(&sceneBounds.Center);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(lightPos, targetPos, up);

	// Transform bounding sphere to light space
	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, V));

	// Orthogonal frustum in light space encloses scene
	float l = sphereCenterLS.x - sceneBounds.Radius;
	float b = sphereCenterLS.y - sceneBounds.Radius;
	float n = sphereCenterLS.z - sceneBounds.Radius;
	float r = sphereCenterLS.x + sceneBounds.Radius;
	float t = sphereCenterLS.y + sceneBounds.Radius;
	float f = sphereCenterLS.z + sceneBounds.Radius;
	XMMATRIX P = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX S = V*P*T;

	XMStoreFloat4x4(&mLightView, V);
	XMStoreFloat4x4(&mLightProj, P);
	XMStoreFloat4x4(&mShadowTransform, S);
}

void ShadowMap::DrawSceneToShadowMap(
	const std::vector<ModelInstanceImpl*>& modelInstances,
	const std::vector<AnimatedInstanceImpl*>& mAnimatedInstances,
	ID3D11DeviceContext* deviceContext,
	ShadowShader* shadowShader,
	SkinnedShadowShader* sShadowShader)
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
				mAnimatedInstances[i]->model->mInstance.FinalTransforms.size());

			for (UINT j = 0; j < mAnimatedInstances[i]->model->mInstance.model->numMeshes; ++j)
			{
				sShadowShader->UpdatePerObj(deviceContext);
				mAnimatedInstances[i]->model->mInstance.model->meshes[j].draw(deviceContext);
			}
		}
	}

	deviceContext->RSSetState(0);
}

XMMATRIX ShadowMap::GetShadowTransform() const
{
	return XMLoadFloat4x4(&mShadowTransform);
}

XMMATRIX ShadowMap::GetLightViewProj() const
{
	return XMMatrixMultiply(XMLoadFloat4x4(&mLightView), XMLoadFloat4x4(&mLightProj));
}
