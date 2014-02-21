#include "DeferredBuffers.h"

// Must be included last!
#include "shared/debug.h"

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
	//formats[DeferredBuffersIndex::LitScene] = DXGI_FORMAT_R8G8B8A8_UNORM;
	formats[DeferredBuffersIndex::Background] = DXGI_FORMAT_R8G8B8A8_UNORM;

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

	// Lit scene HDR buffer
	DXGI_FORMAT format = DXGI_FORMAT_R16G16B16A16_FLOAT;

	textureDesc.Format = format;
	hr = device->CreateTexture2D(&textureDesc, NULL, &mLitSceneRenderTargetTexture);
	if (FAILED(hr))
		return false;

	renderTargetViewDesc.Format = format;
	hr = device->CreateRenderTargetView(mLitSceneRenderTargetTexture, &renderTargetViewDesc, &mLitSceneRenderTargetView);
	if (FAILED(hr))
		return false;

	shaderResourceViewDesc.Format = format;
	hr = device->CreateShaderResourceView(mLitSceneRenderTargetTexture, &shaderResourceViewDesc, &mLitSceneShaderResourceView);
	if (FAILED(hr))
		return false;

	// Background buffer
// 	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
// 	textureDesc.Format = format;
// 	hr = device->CreateTexture2D(&textureDesc, NULL, &mBackgroundRenderTargetTexture);
// 	if (FAILED(hr))
// 		return false;
// 
// 	renderTargetViewDesc.Format = format;
// 	hr = device->CreateRenderTargetView(mBackgroundRenderTargetTexture, &renderTargetViewDesc, &mBackgroundRenderTargetView);
// 	if (FAILED(hr))
// 		return false;
// 
// 	shaderResourceViewDesc.Format = format;
// 	hr = device->CreateShaderResourceView(mBackgroundRenderTargetTexture, &shaderResourceViewDesc, &mBackgroundShaderResourceView);
// 	if (FAILED(hr))
// 		return false;

	// Create the depth/stencil buffer and view
// 	D3D11_TEXTURE2D_DESC depthStencilDesc;
// 	depthStencilDesc.Width = width;						// Texture width in texels
// 	depthStencilDesc.Height = height;					// Texture height in texels
// 	depthStencilDesc.MipLevels = 1;								// Number of mipmap levels
// 	depthStencilDesc.ArraySize = 1;								// Number of textures in texture array
// 	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;	// Texel format
// 
// 	// Set number of multisamples and quality level for the depth/stencil buffer
// 	// This has to match swap chain MSAA values
// 	depthStencilDesc.SampleDesc.Count = 1;
// 	depthStencilDesc.SampleDesc.Quality = 0;
// 
// 	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;			// How the texture will be used
// 	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE;	// Where the resource will be bound to the pipeline
// 	depthStencilDesc.CPUAccessFlags = 0;					// Specify CPU access (Only GPU writes/reads to the depth/buffer)
// 	depthStencilDesc.MiscFlags = 0;							// Optional flags
// 
// 	hr = device->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer);
// 
// 	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
// 	memset(&depthStencilViewDesc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
// 	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
// 	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
// 	depthStencilViewDesc.Texture2D.MipSlice = 0;
// 
// 	hr = device->CreateDepthStencilView(mDepthStencilBuffer, &depthStencilViewDesc, &mDepthStencilView);

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

	//dc->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
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
			ReleaseCOM(mShaderResourceViewArray[i]);
		}

		if (mRenderTargetViewArray[i])
		{
			ReleaseCOM(mRenderTargetViewArray[i]);
		}

		if (mRenderTargetTextureArray[i])
		{
			ReleaseCOM(mRenderTargetTextureArray[i]);
		}
	}

	ReleaseCOM(mLitSceneRenderTargetTexture);
	ReleaseCOM(mLitSceneRenderTargetView);
	ReleaseCOM(mLitSceneShaderResourceView);

// 	ReleaseCOM(mDepthStencilBuffer);
// 	ReleaseCOM(mDepthStencilView);

// 	ReleaseCOM(mBackgroundRenderTargetTexture);
// 	ReleaseCOM(mBackgroundRenderTargetView);
// 	ReleaseCOM(mBackgroundShaderResourceView);
}

ID3D11RenderTargetView* DeferredBuffers::GetRenderTarget(UINT bufferIndex)
{
	return mRenderTargetViewArray[bufferIndex];
}

ID3D11RenderTargetView* DeferredBuffers::GetLitSceneRTV()
{
	return mLitSceneRenderTargetView;
}

ID3D11ShaderResourceView* DeferredBuffers::GetLitSceneSRV()
{
	return mLitSceneShaderResourceView;
}

ID3D11Texture2D* DeferredBuffers::GetLitSceneTexture2D()
{
	return mLitSceneRenderTargetTexture;
}

ID3D11RenderTargetView* DeferredBuffers::GetBackgroundRTV()
{
	return mLitSceneRenderTargetView;
}

ID3D11ShaderResourceView* DeferredBuffers::GetBackgroundSRV()
{
	return mLitSceneShaderResourceView;
}

ID3D11Texture2D* DeferredBuffers::GetBackgroundTexture2D()
{
	return mLitSceneRenderTargetTexture;
}

// ID3D11DepthStencilView* DeferredBuffers::GetDepthStencilView()
// {
// 	return mDepthStencilView;
// }
