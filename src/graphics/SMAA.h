#ifndef SMAA_H_
#define SMAA_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include "ShaderHandler.h"
#include "FullscreenTriangle.h"

using namespace DirectX;

enum SmaaBufferIndex
{
	Edges = 0,
	Blend,
	SmaaBufferCount
};

enum PreCalculatedTexturesIndex
{
	Area = 0,
	Search,
	PerCalcTexCount
};

class SMAA
{
public:
	SMAA();
	~SMAA();

	bool Init(ID3D11Device* device, UINT width, UINT height,
		ID3D11ShaderResourceView* areaTex, ID3D11ShaderResourceView* searchTex,
		FullscreenTriangle* fullscreenTriangle);
	void OnResize(ID3D11Device* device, UINT width, UINT height);

	void SetRenderTargets(ID3D11DeviceContext* dc, ID3D11DepthStencilView* depthStencilView);
	void ClearRenderTargets(ID3D11DeviceContext* dc, DirectX::XMFLOAT4 RGBA, ID3D11DepthStencilView* depthStencilView);

	void SetShaders(SMAAColorEdgeDetectionShader* colorEdge,
		SMAALumaEdgeDetectionShader* lumaEdge,
		SMAADepthEdgeDetectionShader* depthEdge,
		SMAANeighborhoodBlendingShader* neighborhoodBlending,
		SMAABlendingWeightCalculationsShader* blendingWeightCalc);

	ID3D11RenderTargetView* GetRenderTarget(UINT bufferIndex);

	ID3D11ShaderResourceView* GetPreCalculatedTexture(UINT preCalcTexIndex);

	ID3D11ShaderResourceView* GetSRV(UINT view);

	void Run(ID3D11DeviceContext* dc,
		ID3D11ShaderResourceView* sourceGammaSRV,
		ID3D11ShaderResourceView* depthSRV,
		ID3D11ShaderResourceView* velocitySRV,
		ID3D11RenderTargetView* destinationRTV,
		ID3D11DepthStencilView* dsv);

	void Shutdown();

private:
	ID3D11Texture2D* mRenderTargetTextureArray[SmaaBufferIndex::SmaaBufferCount];
	ID3D11RenderTargetView* mRenderTargetViewArray[SmaaBufferIndex::SmaaBufferCount];
	ID3D11ShaderResourceView* mShaderResourceViewArray[SmaaBufferIndex::SmaaBufferCount];

	// Pre-calculated textures
	ID3D11ShaderResourceView* mPreCalculatedTextures[PreCalculatedTexturesIndex::PerCalcTexCount];

	SMAAColorEdgeDetectionShader* mSMAAColorEdgeDetectionShader;
	SMAALumaEdgeDetectionShader* mSMAALumaEdgeDetectionShader;
	SMAADepthEdgeDetectionShader* mSMAADepthEdgeDetectionShader;

	SMAANeighborhoodBlendingShader* mSMAANeighborhoodBlendingShader;
	SMAABlendingWeightCalculationsShader* mSMAABlendingWeightCalculationsShader;

	FullscreenTriangle* mFullscreenTriangle;
};

#endif