#ifndef GRAPHICS_CASCADE_H
#define GRAPHICS_CASCADE_H

#include "ShaderHandler.h"
#include "InstanceImpl.h"
#include "MorphModel.h"

class Cascade
{
public:
	Cascade(ID3D11Device* device, UINT width, UINT height);
	Cascade(const Cascade& other);
	~Cascade(void);

	ID3D11ShaderResourceView* getDepthMapSRV();
	ID3D11DepthStencilView* getDepthMapDSV();

	void BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc);
	void SetResolution(ID3D11Device* device, UINT width, UINT height);
	void SetSplitDepthNear(float zNear);
	void SetSplitDepthFar(float zFar);

	UINT GetWidth() const;
	UINT GetHeight() const;
	float GetSplitDepthNear() const;
	float GetSplitDepthFar() const;
	XMMATRIX GetShadowTransform() const;
	XMMATRIX GetLightViewProj() const;
	XMMATRIX GetLightWorld() const;
	XMMATRIX GetLightView() const;
	XMMATRIX GetLightProj() const;

	//Set world, view, projection and shadowtransform matrices
	void SetShadowMatrices(const XMMATRIX& lightView, const XMMATRIX& proj, XMVECTOR lightPos);

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
	float zNearSplit;
	float zFarSplit;

	UINT mWidth;
	UINT mHeight;

	XMFLOAT4X4 mLightWorld;
	XMFLOAT4X4 mLightView;
	XMFLOAT4X4 mLightProj;
	XMFLOAT4X4 mShadowTransform;

	ID3D11ShaderResourceView* mDepthMapSRV;
	ID3D11DepthStencilView* mDepthMapDSV;

	D3D11_VIEWPORT mViewport;

	//ID3D11RenderTargetView* mRenderTargetView;
	//ID3D11Texture2D* mRenderTargetTexture;

	bool CreateCascade(ID3D11Device* device, UINT width, UINT height);
};

#endif