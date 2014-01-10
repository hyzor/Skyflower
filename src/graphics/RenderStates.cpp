#include "RenderStates.h"

ID3D11RasterizerState* RenderStates::mDefaultRS = 0;
ID3D11RasterizerState* RenderStates::mWireframeRS = 0;
ID3D11RasterizerState* RenderStates::mNoCullRS = 0;
ID3D11RasterizerState* RenderStates::mDepthBiasRS = 0;

ID3D11SamplerState* RenderStates::mLinearSS = 0;
ID3D11SamplerState* RenderStates::mAnisotropicSS = 0;
ID3D11SamplerState* RenderStates::mComparisonSS = 0;

ID3D11DepthStencilState* RenderStates::mLessEqualDSS = 0;
ID3D11DepthStencilState* RenderStates::mDefaultDDS = 0;

ID3D11BlendState* RenderStates::mDefaultBS = 0;

void RenderStates::InitAll(ID3D11Device* device)
{
	//-----------------------------------------------------------
	// Rasterizer states
	//-----------------------------------------------------------
	// Default rasterizer state
	D3D11_RASTERIZER_DESC defaultRSdesc;
	ZeroMemory(&defaultRSdesc, sizeof(D3D11_RASTERIZER_DESC));
	defaultRSdesc.AntialiasedLineEnable = false;
	defaultRSdesc.CullMode = D3D11_CULL_BACK;
	defaultRSdesc.DepthBias = 0;
	defaultRSdesc.DepthBiasClamp = 0.0f;
	defaultRSdesc.DepthClipEnable = true;
	defaultRSdesc.FillMode = D3D11_FILL_SOLID;
	defaultRSdesc.FrontCounterClockwise = false;
	defaultRSdesc.MultisampleEnable = false;
	defaultRSdesc.ScissorEnable = false;
	defaultRSdesc.SlopeScaledDepthBias = 0.0f;

	device->CreateRasterizerState(&defaultRSdesc, &mDefaultRS);

	// Wireframe rasterizer state
	D3D11_RASTERIZER_DESC wireframeRSdesc;
	ZeroMemory(&wireframeRSdesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeRSdesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeRSdesc.CullMode = D3D11_CULL_BACK;
	wireframeRSdesc.FrontCounterClockwise = false;
	wireframeRSdesc.DepthClipEnable = true;

	device->CreateRasterizerState(&wireframeRSdesc, &mWireframeRS);

	// No culling rasterizer state
	D3D11_RASTERIZER_DESC noCullRSdesc;
	ZeroMemory(&noCullRSdesc, sizeof(D3D11_RASTERIZER_DESC));
	noCullRSdesc.FillMode = D3D11_FILL_SOLID;
	noCullRSdesc.CullMode = D3D11_CULL_NONE;
	noCullRSdesc.FrontCounterClockwise = false;
	noCullRSdesc.DepthClipEnable = true;

	device->CreateRasterizerState(&noCullRSdesc, &mNoCullRS);

	D3D11_RASTERIZER_DESC depthBiasRSdesc;
	ZeroMemory(&depthBiasRSdesc, sizeof(D3D11_RASTERIZER_DESC));
	depthBiasRSdesc.DepthBias = 1000;
	depthBiasRSdesc.DepthBiasClamp = 0.0f;
	depthBiasRSdesc.SlopeScaledDepthBias = 1.0f;
	depthBiasRSdesc.FillMode = D3D11_FILL_SOLID;
	depthBiasRSdesc.CullMode = D3D11_CULL_BACK;

	device->CreateRasterizerState(&depthBiasRSdesc, &mDepthBiasRS);

	//-----------------------------------------------------------
	// Sampler states
	//-----------------------------------------------------------
	// Linear sampler state
	D3D11_SAMPLER_DESC linearSSdesc;
	ZeroMemory(&linearSSdesc, sizeof(D3D11_SAMPLER_DESC));
	linearSSdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	linearSSdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	linearSSdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	linearSSdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	linearSSdesc.MipLODBias = 0.0f;
	linearSSdesc.MaxAnisotropy = 1;
	linearSSdesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	linearSSdesc.BorderColor[0] = 0;
	linearSSdesc.BorderColor[1] = 0;
	linearSSdesc.BorderColor[2] = 0;
	linearSSdesc.BorderColor[3] = 0;
	linearSSdesc.MinLOD = 0;
	linearSSdesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&linearSSdesc, &mLinearSS);

	// Anisotropic sampler state
	D3D11_SAMPLER_DESC anisotropicSSdesc;
	ZeroMemory(&anisotropicSSdesc, sizeof(D3D11_SAMPLER_DESC));
	anisotropicSSdesc.Filter = D3D11_FILTER_ANISOTROPIC;
	anisotropicSSdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	anisotropicSSdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	anisotropicSSdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	anisotropicSSdesc.MipLODBias = 0.0f;
	anisotropicSSdesc.MaxAnisotropy = 4;
	anisotropicSSdesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	anisotropicSSdesc.BorderColor[0] = 0;
	anisotropicSSdesc.BorderColor[1] = 0;
	anisotropicSSdesc.BorderColor[2] = 0;
	anisotropicSSdesc.BorderColor[3] = 0;
	anisotropicSSdesc.MinLOD = 0;
	anisotropicSSdesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&anisotropicSSdesc, &mAnisotropicSS);

	//comparison filter sampler state
	D3D11_SAMPLER_DESC comparisonSSdesc;
	ZeroMemory(&comparisonSSdesc, sizeof(D3D11_SAMPLER_DESC));
	comparisonSSdesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	comparisonSSdesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSSdesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSSdesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSSdesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	comparisonSSdesc.BorderColor[0] = 0.0f;
	comparisonSSdesc.BorderColor[1] = 0.0f;
	comparisonSSdesc.BorderColor[2] = 0.0f;
	comparisonSSdesc.BorderColor[3] = 0.0f;

	device->CreateSamplerState(&comparisonSSdesc, &mComparisonSS);

	//-----------------------------------------------------------
	// Depth stencil states
	//-----------------------------------------------------------
	// Less equal depth stencil state
	D3D11_DEPTH_STENCIL_DESC lessEqualDSSdesc;
	ZeroMemory(&lessEqualDSSdesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	lessEqualDSSdesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	device->CreateDepthStencilState(&lessEqualDSSdesc, &mLessEqualDSS);

	// Less equal depth stencil state
	D3D11_DEPTH_STENCIL_DESC defaultDSSdesc;
	ZeroMemory(&defaultDSSdesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	defaultDSSdesc.DepthEnable = TRUE;
	defaultDSSdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	defaultDSSdesc.DepthFunc = D3D11_COMPARISON_LESS;
	defaultDSSdesc.StencilEnable = FALSE;
	defaultDSSdesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	defaultDSSdesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	defaultDSSdesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	defaultDSSdesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	defaultDSSdesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	defaultDSSdesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	defaultDSSdesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	defaultDSSdesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	defaultDSSdesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	defaultDSSdesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	device->CreateDepthStencilState(&defaultDSSdesc, &mDefaultDDS);

	//-----------------------------------------------------------
	// Blend states
	//-----------------------------------------------------------
	// Default blend state
	D3D11_BLEND_DESC defaultBSdesc;
	ZeroMemory(&defaultBSdesc, sizeof(D3D11_BLEND_DESC));
	defaultBSdesc.AlphaToCoverageEnable = FALSE;
	defaultBSdesc.IndependentBlendEnable = FALSE;
	defaultBSdesc.RenderTarget[0].BlendEnable = FALSE;
	defaultBSdesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	defaultBSdesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	defaultBSdesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	defaultBSdesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	defaultBSdesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	defaultBSdesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	defaultBSdesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
}

void RenderStates::DestroyAll()
{
	ReleaseCOM(mDefaultRS);
	ReleaseCOM(mWireframeRS);
	ReleaseCOM(mNoCullRS);
	ReleaseCOM(mDepthBiasRS);

	ReleaseCOM(mLinearSS);
	ReleaseCOM(mAnisotropicSS);
	ReleaseCOM(mComparisonSS);

	ReleaseCOM(mLessEqualDSS);
}
