//------------------------------------------------------------------------------------
// File: ShadowMap.h
//
// This class encapsulates a depth/stencil buffer, views and viewport for
// shadow mapping
//------------------------------------------------------------------------------------

#ifndef GRAPHICS_SHADOWMAP_H
#define GRAPHICS_SHADOWMAP_H

#include "ShaderHandler.h"
#include "InstanceImpl.h"
#include <vector>
#include "MorphModel.h"

class ShadowMap
{
public:
	ShadowMap(ID3D11Device* device, UINT width, UINT height);
	~ShadowMap(void);

	ID3D11ShaderResourceView* getDepthMapSRV();
	ID3D11DepthStencilView* getDepthMapDSV();

	void BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc);
	void SetResolution(ID3D11Device* device, UINT width, UINT height);

	UINT GetWidth() const;
	UINT GetHeight() const;
	XMMATRIX GetShadowTransform() const;
	XMMATRIX GetLightViewProj() const;
	XMMATRIX GetLightWorld() const;
	XMMATRIX GetLightView() const;
	XMMATRIX GetLightProj() const;

	void BuildShadowTransform(const DLight& light, const DirectX::BoundingSphere& sceneBounds);

	void DrawSceneToShadowMap(
		const std::vector<ModelInstanceImpl*>& modelInstances,
		const std::vector<AnimatedInstanceImpl*>& mAnimatedInstances,
		ID3D11DeviceContext* deviceContext,
		ShadowShader* shadowShader,
		SkinnedShadowShader* skinnedShadowShader);

	void DrawSceneToShadowMap(
		const std::vector<ModelInstanceImpl*>& modelInstances,
		const std::vector<AnimatedInstanceImpl*>& mAnimatedInstances,
		const std::vector<MorphModelInstanceImpl*>& mMorphInstances,
		ID3D11DeviceContext* deviceContext,
		ShadowShader* shadowShader,
		SkinnedShadowShader* skinnedShadowShader,
		ShadowMorphShader* shadowMorphShader);

private:
	UINT mWidth;
	UINT mHeight;

	XMFLOAT4X4 mLightWorld;
	XMFLOAT4X4 mLightView;
	XMFLOAT4X4 mLightProj;
	XMFLOAT4X4 mShadowTransform;

	ID3D11ShaderResourceView* mDepthMapSRV;
	ID3D11DepthStencilView* mDepthMapDSV;

	D3D11_VIEWPORT mViewport;

	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11Texture2D* mRenderTargetTexture;

	bool CreateShadowMap(ID3D11Device* device, UINT width, UINT height);
};

#endif