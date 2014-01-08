#include "DeferredBuffers.h"

DeferredBuffers::DeferredBuffers()
{
	for (UINT i = 0; i < BUFFER_COUNT; ++i)
	{
		mRenderTargetTextureArray[i] = 0;
		mRenderTargetViewArray[i] = 0;
		mShaderResourceViewArray[i] = 0;
	}
}

DeferredBuffers::~DeferredBuffers()
{
	for (UINT i = 0; i < BUFFER_COUNT; i++)
	{
		if (mShaderResourceViewArray[i])
		{
			mShaderResourceViewArray[i]->Release();
			mShaderResourceViewArray[i] = 0;
		}

		if (mRenderTargetViewArray[i])
		{
			mRenderTargetViewArray[i]->Release();
			mRenderTargetViewArray[i] = 0;
		}

		if (mRenderTargetTextureArray[i])
		{
			mRenderTargetTextureArray[i]->Release();
			mRenderTargetTextureArray[i] = 0;
		}
	}
}

bool DeferredBuffers::Init(ID3D11Device* device, UINT width, UINT height, float nearZ, float farZ)
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	// Setup render target texture description
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create render target texture
	for (UINT i = 0; i < BUFFER_COUNT; ++i)
	{
		hr = device->CreateTexture2D(&textureDesc, NULL, &mRenderTargetTextureArray[i]);

		if (FAILED(hr))
			return false;
	}

	// Render target view description
	ZeroMemory(&renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create render target views
	for (UINT i = 0; i < BUFFER_COUNT; ++i)
	{
		hr = device->CreateRenderTargetView(mRenderTargetTextureArray[i], &renderTargetViewDesc, &mRenderTargetViewArray[i]);

		if (FAILED(hr))
			return false;
	}

	// Shader resource view description
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	for (UINT i = 0; i < BUFFER_COUNT; ++i)
	{
		hr = device->CreateShaderResourceView(mRenderTargetTextureArray[i], &shaderResourceViewDesc, &mShaderResourceViewArray[i]);

		if (FAILED(hr))
			return false;
	}

	return true;
}

void DeferredBuffers::SetRenderTargets(ID3D11DeviceContext* dc, ID3D11DepthStencilView* depthStencilView)
{
	dc->OMSetRenderTargets(BUFFER_COUNT, mRenderTargetViewArray, depthStencilView);
}

void DeferredBuffers::ClearRenderTargets(ID3D11DeviceContext* dc, DirectX::XMFLOAT4 RGBA, ID3D11DepthStencilView* depthStencilView)
{
	float color[4];

	color[0] = RGBA.x;
	color[1] = RGBA.y;
	color[2] = RGBA.z;
	color[3] = RGBA.w;

	for (UINT i = 0; i < BUFFER_COUNT; ++i)
	{
		dc->ClearRenderTargetView(mRenderTargetViewArray[i], color);
	}

	dc->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11ShaderResourceView* DeferredBuffers::GetSRV(int view)
{
	return mShaderResourceViewArray[view];
}

void DeferredBuffers::OnResize(UINT width, UINT height)
{

}
