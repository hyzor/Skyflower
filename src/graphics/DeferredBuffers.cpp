#include "DeferredBuffers.h"

DeferredBuffers::DeferredBuffers()
{
	for (UINT i = 0; i < DeferredBuffersIndex::Count; ++i)
	{
		mRenderTargetTextureArray[i] = 0;
		mRenderTargetViewArray[i] = 0;
		mShaderResourceViewArray[i] = 0;
	}
}

DeferredBuffers::~DeferredBuffers()
{
	Shutdown();
}

bool DeferredBuffers::Init(ID3D11Device* device, UINT width, UINT height)
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	DXGI_FORMAT formats[DeferredBuffersIndex::Count];
	formats[DeferredBuffersIndex::Diffuse] = DXGI_FORMAT_R8G8B8A8_UNORM;
	formats[DeferredBuffersIndex::Normal] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	formats[DeferredBuffersIndex::Specular] = DXGI_FORMAT_R8G8B8A8_UNORM;
	formats[DeferredBuffersIndex::Velocity] = DXGI_FORMAT_R8G8_UNORM;

	// Setup render target texture description
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create render target texture
	for (UINT i = 0; i < DeferredBuffersIndex::Count; ++i)
	{
		textureDesc.Format = formats[i];
		hr = device->CreateTexture2D(&textureDesc, NULL, &mRenderTargetTextureArray[i]);

		if (FAILED(hr))
			return false;
	}

	// Render target view description
	ZeroMemory(&renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create render target views
	for (UINT i = 0; i < DeferredBuffersIndex::Count; ++i)
	{
		renderTargetViewDesc.Format = formats[i];
		hr = device->CreateRenderTargetView(mRenderTargetTextureArray[i], &renderTargetViewDesc, &mRenderTargetViewArray[i]);

		if (FAILED(hr))
			return false;
	}

	// Shader resource view description
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	for (UINT i = 0; i < DeferredBuffersIndex::Count; ++i)
	{
		shaderResourceViewDesc.Format = formats[i];
		hr = device->CreateShaderResourceView(mRenderTargetTextureArray[i], &shaderResourceViewDesc, &mShaderResourceViewArray[i]);

		if (FAILED(hr))
			return false;
	}

	return true;
}

void DeferredBuffers::SetRenderTargets(ID3D11DeviceContext* dc, ID3D11DepthStencilView* depthStencilView)
{
	dc->OMSetRenderTargets(DeferredBuffersIndex::Count, mRenderTargetViewArray, depthStencilView);
}

void DeferredBuffers::ClearRenderTargets(ID3D11DeviceContext* dc, DirectX::XMFLOAT4 RGBA, ID3D11DepthStencilView* depthStencilView)
{
	float color[4];

	color[0] = RGBA.x;
	color[1] = RGBA.y;
	color[2] = RGBA.z;
	color[3] = RGBA.w;

	for (UINT i = 0; i < DeferredBuffersIndex::Count; ++i)
	{
		dc->ClearRenderTargetView(mRenderTargetViewArray[i], color);
	}

	dc->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11ShaderResourceView* DeferredBuffers::GetSRV(int view)
{
	return mShaderResourceViewArray[view];
}

void DeferredBuffers::OnResize(ID3D11Device* device, UINT width, UINT height)
{
	Shutdown();
	Init(device, width, height);
}

void DeferredBuffers::Shutdown()
{
	for (UINT i = 0; i < DeferredBuffersIndex::Count; i++)
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
