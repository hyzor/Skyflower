#ifndef DEFERREDBUFFERS_H_
#define DEFERREDBUFFERS_H_

#include <d3d11.h>
#include <DirectXMath.h>

enum DeferredBuffersIndex
{
	Diffuse = 0,
	Normal,
	Specular,
	Velocity,
	//LitScene,
	Count
};

class DeferredBuffers
{
public:
	DeferredBuffers();
	~DeferredBuffers();

	bool Init(ID3D11Device* device, UINT width, UINT height);
	void OnResize(ID3D11Device* device, UINT width, UINT height);

	void SetRenderTargets(ID3D11DeviceContext* dc, ID3D11DepthStencilView* depthStencilView);
	void ClearRenderTargets(ID3D11DeviceContext* dc, DirectX::XMFLOAT4 RGBA, ID3D11DepthStencilView* depthStencilView);

	ID3D11RenderTargetView* GetRenderTarget(UINT bufferIndex);

	/*
	ID3D11RenderTargetView* GetLitSceneRTV();
	ID3D11ShaderResourceView* GetLitSceneSRV();
	*/

	ID3D11ShaderResourceView* GetSRV(int view);

	void Shutdown();

private:
	ID3D11Texture2D* mRenderTargetTextureArray[DeferredBuffersIndex::Count];
	ID3D11RenderTargetView* mRenderTargetViewArray[DeferredBuffersIndex::Count];
	ID3D11ShaderResourceView* mShaderResourceViewArray[DeferredBuffersIndex::Count];

	/*
	ID3D11Texture2D* mLitSceneTexture;
	ID3D11RenderTargetView* mLitSceneRenderTargetView;
	ID3D11ShaderResourceView* mLitSceneShaderResourceView;
	*/

// 	ID3D11DepthStencilView* mDepthStencilView;
// 	ID3D11Texture2D* mDepthStencilBuffer;
};

#endif