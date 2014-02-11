#include "SmaaBuffers.h"


SmaaBuffers::SmaaBuffers()
{
	for (UINT i = 0; i < SmaaBufferIndex::SmaaBufferCount; ++i)
	{
		mRenderTargetTextureArray[i] = 0;
		mRenderTargetViewArray[i] = 0;
		mShaderResourceViewArray[i] = 0;
	}
}


SmaaBuffers::~SmaaBuffers()
{
	Shutdown();
}

/*
bool SmaaBuffers::Init(ID3D11Device* device, UINT width, UINT height)
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	DXGI_FORMAT formats[SmaaBufferIndex::Count];
	formats[SmaaBufferIndex::Edges] = DXGI_FORMAT_R8G8B8A8_UNORM;
	formats[SmaaBufferIndex::Blend] = DXGI_FORMAT_R8G8B8A8_UNORM;

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
	for (UINT i = 0; i < SmaaBufferIndex::Count; ++i)
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
	for (UINT i = 0; i < SmaaBufferIndex::Count; ++i)
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

	for (UINT i = 0; i < SmaaBufferIndex::Count; ++i)
	{
		shaderResourceViewDesc.Format = formats[i];
		hr = device->CreateShaderResourceView(mRenderTargetTextureArray[i], &shaderResourceViewDesc, &mShaderResourceViewArray[i]);

		if (FAILED(hr))
			return false;
	}

	return true;
}
*/

bool SmaaBuffers::Init(ID3D11Device* device, UINT width, UINT height, ID3D11ShaderResourceView* areaTex, ID3D11ShaderResourceView* searchTex)
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	DXGI_FORMAT formats[SmaaBufferIndex::SmaaBufferCount];
	formats[SmaaBufferIndex::Edges] = DXGI_FORMAT_R8G8B8A8_UNORM;
	formats[SmaaBufferIndex::Blend] = DXGI_FORMAT_R8G8B8A8_UNORM;

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
	for (UINT i = 0; i < SmaaBufferIndex::SmaaBufferCount; ++i)
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
	for (UINT i = 0; i < SmaaBufferIndex::SmaaBufferCount; ++i)
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

	for (UINT i = 0; i < SmaaBufferIndex::SmaaBufferCount; ++i)
	{
		shaderResourceViewDesc.Format = formats[i];
		hr = device->CreateShaderResourceView(mRenderTargetTextureArray[i], &shaderResourceViewDesc, &mShaderResourceViewArray[i]);

		if (FAILED(hr))
			return false;
	}

	mPreCalculatedTextures[PreCalculatedTexturesIndex::Area] = areaTex;
	mPreCalculatedTextures[PreCalculatedTexturesIndex::Search] = searchTex;

	return true;
}

void SmaaBuffers::OnResize(ID3D11Device* device, UINT width, UINT height)
{
	Shutdown();
	Init(device, width, height, mPreCalculatedTextures[PreCalculatedTexturesIndex::Area], mPreCalculatedTextures[PreCalculatedTexturesIndex::Search]);
}

void SmaaBuffers::SetRenderTargets(ID3D11DeviceContext* dc, ID3D11DepthStencilView* depthStencilView)
{
	dc->OMSetRenderTargets(SmaaBufferIndex::SmaaBufferCount, mRenderTargetViewArray, depthStencilView);
}

void SmaaBuffers::ClearRenderTargets(ID3D11DeviceContext* dc, DirectX::XMFLOAT4 RGBA, ID3D11DepthStencilView* depthStencilView)
{
	float color[4];

	color[0] = RGBA.x;
	color[1] = RGBA.y;
	color[2] = RGBA.z;
	color[3] = RGBA.w;

	for (UINT i = 0; i < SmaaBufferIndex::SmaaBufferCount; ++i)
	{
		dc->ClearRenderTargetView(mRenderTargetViewArray[i], color);
	}

	dc->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11RenderTargetView* SmaaBuffers::GetRenderTarget(UINT bufferIndex)
{
	return mRenderTargetViewArray[bufferIndex];
}

ID3D11ShaderResourceView* SmaaBuffers::GetSRV(UINT view)
{
	return mShaderResourceViewArray[view];
}

void SmaaBuffers::Shutdown()
{
	for (UINT i = 0; i < SmaaBufferIndex::SmaaBufferCount; i++)
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

ID3D11ShaderResourceView* SmaaBuffers::GetPreCalculatedTexture(UINT preCalcTexIndex)
{
	return mPreCalculatedTextures[preCalcTexIndex];
}
