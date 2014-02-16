#include "SMAA.h"


SMAA::SMAA()
{
	for (UINT i = 0; i < SmaaBufferIndex::SmaaBufferCount; ++i)
	{
		mRenderTargetTextureArray[i] = 0;
		mRenderTargetViewArray[i] = 0;
		mShaderResourceViewArray[i] = 0;
	}
}


SMAA::~SMAA()
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

bool SMAA::Init(ID3D11Device* device, UINT width, UINT height,
	ID3D11ShaderResourceView* areaTex, ID3D11ShaderResourceView* searchTex,
	FullscreenTriangle* fullscreenTriangle)
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

	mFullscreenTriangle = fullscreenTriangle;

	mSMAAColorEdgeDetectionShader->SetScreenDimensions(height, width);
	mSMAADepthEdgeDetectionShader->SetScreenDimensions(height, width);
	mSMAALumaEdgeDetectionShader->SetScreenDimensions(height, width);
	mSMAABlendingWeightCalculationsShader->SetScreenDimensions(height, width);
	mSMAANeighborhoodBlendingShader->SetScreenDimensions(height, width);

	return true;
}

void SMAA::OnResize(ID3D11Device* device, UINT width, UINT height)
{
	Shutdown();
	Init(device, width, height,
		mPreCalculatedTextures[PreCalculatedTexturesIndex::Area],
		mPreCalculatedTextures[PreCalculatedTexturesIndex::Search],
		mFullscreenTriangle);

	mSMAAColorEdgeDetectionShader->SetScreenDimensions(height, width);
	mSMAADepthEdgeDetectionShader->SetScreenDimensions(height, width);
	mSMAALumaEdgeDetectionShader->SetScreenDimensions(height, width);
	mSMAABlendingWeightCalculationsShader->SetScreenDimensions(height, width);
	mSMAANeighborhoodBlendingShader->SetScreenDimensions(height, width);
}

void SMAA::SetRenderTargets(ID3D11DeviceContext* dc, ID3D11DepthStencilView* depthStencilView)
{
	dc->OMSetRenderTargets(SmaaBufferIndex::SmaaBufferCount, mRenderTargetViewArray, depthStencilView);
}

void SMAA::ClearRenderTargets(ID3D11DeviceContext* dc, DirectX::XMFLOAT4 RGBA, ID3D11DepthStencilView* depthStencilView)
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

	//dc->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11RenderTargetView* SMAA::GetRenderTarget(UINT bufferIndex)
{
	return mRenderTargetViewArray[bufferIndex];
}

ID3D11ShaderResourceView* SMAA::GetSRV(UINT view)
{
	return mShaderResourceViewArray[view];
}

void SMAA::Shutdown()
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

ID3D11ShaderResourceView* SMAA::GetPreCalculatedTexture(UINT preCalcTexIndex)
{
	return mPreCalculatedTextures[preCalcTexIndex];
}

void SMAA::Run(ID3D11DeviceContext* dc,
	ID3D11ShaderResourceView* sourceGammaSRV,
	ID3D11ShaderResourceView* depthSRV,
	ID3D11ShaderResourceView* velocitySRV,
	ID3D11RenderTargetView* destinationRTV,
	ID3D11DepthStencilView* dsv)
{
	// Clear render targets
	ClearRenderTargets(dc, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), dsv);

	// Clear depth/stencil view
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	float blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	//------------------------------------------------------------------------------
	// Edge detection pass
	//------------------------------------------------------------------------------
	// Disable depth replace stencil
	dc->OMSetDepthStencilState(RenderStates::mDepthDisabledStencilReplaceDSS, 1);

	// Edge detection passes uses no blending
	dc->OMSetBlendState(RenderStates::mDefaultBS, blendFactor, 0xffffffff);
	dc->OMSetRenderTargets(1, &mRenderTargetViewArray[SmaaBufferIndex::Edges], dsv);

	// Luma edge detection
	mSMAALumaEdgeDetectionShader->SetActive(dc);
	mSMAALumaEdgeDetectionShader->SetColorTexture(dc, sourceGammaSRV);
	mSMAALumaEdgeDetectionShader->SetPredicationTex(dc, nullptr);
	mSMAALumaEdgeDetectionShader->UpdatePerFrame(dc);

	// Depth edge detection
// 	mSMAADepthEdgeDetectionShader->SetActive(dc);
// 	mSMAADepthEdgeDetectionShader->SetDepthTexture(dc, depthSRV);
// 	mSMAADepthEdgeDetectionShader->UpdatePerFrame(dc);

	// Color edge detection
// 	mSMAAColorEdgeDetectionShader->SetActive(dc);
// 	mSMAAColorEdgeDetectionShader->SetColorTexture(dc, sourceGammaSRV);
// 	mSMAAColorEdgeDetectionShader->SetPredicationTex(dc, nullptr);
// 	mSMAAColorEdgeDetectionShader->UpdatePerFrame(dc);
	
	mFullscreenTriangle->Draw(dc);

	//mSMAADepthEdgeDetectionShader->SetDepthTexture(dc, nullptr);
	mSMAALumaEdgeDetectionShader->SetColorTexture(dc, nullptr);
	mSMAALumaEdgeDetectionShader->SetPredicationTex(dc, nullptr);

	//------------------------------------------------------------------------------
	// Blending weights calculation pass
	//------------------------------------------------------------------------------
	// Disable depth use stencil
	dc->OMSetDepthStencilState(RenderStates::mDepthDisabledStencilUseDSS, 1);

	// Blending weights calculations pass uses no blending
	dc->OMSetBlendState(RenderStates::mDefaultBS, blendFactor, 0xffffffff);
	dc->OMSetRenderTargets(1, &mRenderTargetViewArray[SmaaBufferIndex::Blend], dsv);

	mSMAABlendingWeightCalculationsShader->SetActive(dc);
	mSMAABlendingWeightCalculationsShader->SetEdgesTexture(dc, mShaderResourceViewArray[SmaaBufferIndex::Edges]);
	mSMAABlendingWeightCalculationsShader->SetAreaTexture(dc, mPreCalculatedTextures[PreCalculatedTexturesIndex::Area]);
	mSMAABlendingWeightCalculationsShader->SetSearchTexture(dc, mPreCalculatedTextures[PreCalculatedTexturesIndex::Search]);
	mSMAABlendingWeightCalculationsShader->UpdatePerFrame(dc);

	mFullscreenTriangle->Draw(dc);

	mSMAABlendingWeightCalculationsShader->SetEdgesTexture(dc, nullptr);
	mSMAABlendingWeightCalculationsShader->SetAreaTexture(dc, nullptr);
	mSMAABlendingWeightCalculationsShader->SetSearchTexture(dc, nullptr);
	
	//------------------------------------------------------------------------------
	// Neighborhood blending pass
	//------------------------------------------------------------------------------
	// Disable depth and stencil
	dc->OMSetDepthStencilState(RenderStates::mDepthStencilDisabledDSS, 0);

	// If using SMAA 1x, no blending is used
	// Blending is used for all the other modes
	dc->OMSetBlendState(RenderStates::mDefaultBS, blendFactor, 0xffffffff);
	dc->OMSetRenderTargets(1, &destinationRTV, dsv);

	mSMAANeighborhoodBlendingShader->SetActive(dc);
	mSMAANeighborhoodBlendingShader->SetBlendTex(dc, mShaderResourceViewArray[SmaaBufferIndex::Blend]);
	mSMAANeighborhoodBlendingShader->SetColorTexture(dc, sourceGammaSRV);
	mSMAANeighborhoodBlendingShader->SetVelocityTex(dc, velocitySRV);
	mSMAANeighborhoodBlendingShader->UpdatePerFrame(dc);

	mFullscreenTriangle->Draw(dc);

	mSMAANeighborhoodBlendingShader->SetBlendTex(dc, nullptr);
	mSMAANeighborhoodBlendingShader->SetColorTexture(dc, nullptr);
	mSMAANeighborhoodBlendingShader->SetVelocityTex(dc, nullptr);

	// Reset render target
	dc->OMSetRenderTargets(0, nullptr, nullptr);
}

void SMAA::SetShaders(SMAAColorEdgeDetectionShader* colorEdge,
	SMAALumaEdgeDetectionShader* lumaEdge,
	SMAADepthEdgeDetectionShader* depthEdge,
	SMAANeighborhoodBlendingShader* neighborhoodBlending,
	SMAABlendingWeightCalculationsShader* blendingWeightCalc)
{
	mSMAAColorEdgeDetectionShader = colorEdge;
	mSMAALumaEdgeDetectionShader = lumaEdge;
	mSMAADepthEdgeDetectionShader = depthEdge;
	mSMAANeighborhoodBlendingShader = neighborhoodBlending;
	mSMAABlendingWeightCalculationsShader = blendingWeightCalc;
}