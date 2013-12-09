//------------------------------------------------------------------------------------
// File: Effects.h
//
// Encapsulates all the effects (.fx files)
//------------------------------------------------------------------------------------

#ifndef EFFECTS_H
#define EFFECTS_H

#include "d3dUtilities.h"
#include "LightDef.h"

class Effect
{
public:
	Effect(ID3D11Device* device, const std::wstring& filename);
	virtual ~Effect();

private:
	Effect(const Effect& rhs);
	Effect& operator=(const Effect& rhs);

protected:
	ID3DX11Effect* mFX;
};

//====================================================================
// Basic effect
//====================================================================
class BasicEffect : public Effect
{
public:
	BasicEffect(ID3D11Device* device, const std::wstring& filename);
	~BasicEffect();

	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProjTex(CXMMATRIX M)               { WorldViewProjTex->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)              { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }

	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }

	void SetDirLights(const DirectionalLight* lights)   { DirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }
	void SetPointLight(const PointLight* light)			{ PointLight->SetRawValue(light, 0, sizeof(PointLight)); }

	void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }

	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }
	void SetCubeMap(ID3D11ShaderResourceView* tex) { CubeMap->SetResource(tex); }

	void SetShadowMap(ID3D11ShaderResourceView* tex) { ShadowMap->SetResource(tex); }
	void SetShadowTransform(CXMMATRIX M) { ShadowTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }

	void SetFogRange(float f) { FogRange->SetFloat(f); }
	void SetFogStart(float f) { FogStart->SetFloat(f); }
	void SetFogColor(const FXMVECTOR v) { FogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }

	// Techniques
	ID3DX11EffectTechnique* PointLight1Tech;
	ID3DX11EffectTechnique* DirLights3Tech;
	ID3DX11EffectTechnique* PointLight1TexTech;
	ID3DX11EffectTechnique* DirLights3TexTech;
	ID3DX11EffectTechnique* DirLights3TexAlphaClipTech;
	ID3DX11EffectTechnique* DirLights3FogTech;
	ID3DX11EffectTechnique* DirLights3FogTexTech;
	ID3DX11EffectTechnique* DirLights3FogReflectionTech;

	// Matrices
	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* WorldViewProjTex;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;

	// Eye position
	ID3DX11EffectVectorVariable* EyePosW;

	// Material
	ID3DX11EffectVariable* Mat;

	// Lights
	ID3DX11EffectVariable* PointLight;
	ID3DX11EffectVariable* DirLights;

	// Shadow mapping
	ID3DX11EffectShaderResourceVariable* ShadowMap;
	ID3DX11EffectMatrixVariable* ShadowTransform;

	// Texture
	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* CubeMap;

	// Fogging
	ID3DX11EffectScalarVariable* FogRange;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectVectorVariable* FogColor;
};

//==============================================================================
// Normal map effect
//==============================================================================
class NormalMapEffect : public Effect
{
public:
	NormalMapEffect(ID3D11Device* device, const std::wstring& filename);
	~NormalMapEffect();

	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProjTex(CXMMATRIX M)               { WorldViewProjTex->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)              { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetInMenu(bool menu)							{ inMenu->SetBool(menu); }

	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }

	void SetDirLights(const DirectionalLight* lights)   { dirLights->SetRawValue(lights, 0, 3 * sizeof(DirectionalLight)); }
	void SetPointLights(const PointLight* lights)		{ PointLights->SetRawValue(lights, 0, 12 * sizeof(PointLight)); }

	void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }

	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }
	void SetCubeMap(ID3D11ShaderResourceView* tex) { CubeMap->SetResource(tex); }
	void setNormalMap(ID3D11ShaderResourceView* tex) { NormalMap->SetResource(tex); }

	// Shadow map
	void SetShadowMap(ID3D11ShaderResourceView* tex) { shadowMap->SetResource(tex); }
	void SetShadowTransform(CXMMATRIX M) { shadowTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }

	// Fog
	void SetFogRange(float f) { fogRange->SetFloat(f); }
	void SetFogStart(float f) { fogStart->SetFloat(f); }
	void SetFogColor(const FXMVECTOR v) { fogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }

	void SetBoneTransforms(const XMFLOAT4X4* M, int cnt) { BoneTransforms->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt); }

	// Techniques
	ID3DX11EffectTechnique* DirLights3TexTech;
	ID3DX11EffectTechnique* DirLights3TexAlphaClipTech;
	ID3DX11EffectTechnique* DirLights3TexSkinnedTech;

	ID3DX11EffectTechnique* DirLights3PointLights12TexAlphaClipTech;

	ID3DX11EffectTechnique* DirLights3PointLights12TexAlphaClipSkinnedTech;

	// Matrices
	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* WorldViewProjTex;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;

	ID3DX11EffectMatrixVariable* BoneTransforms;

	// Eye position
	ID3DX11EffectVectorVariable* EyePosW;

	// Material
	ID3DX11EffectVariable* Mat;

	// Lights
	ID3DX11EffectVariable* dirLights;
	ID3DX11EffectVariable* PointLights;

	// Shadow mapping
	ID3DX11EffectShaderResourceVariable* shadowMap;
	ID3DX11EffectMatrixVariable* shadowTransform;

	// Texture
	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* CubeMap;
	ID3DX11EffectShaderResourceVariable* NormalMap;

	// Fogging
	ID3DX11EffectScalarVariable* fogRange;
	ID3DX11EffectScalarVariable* fogStart;
	ID3DX11EffectVectorVariable* fogColor;

	// Menu
	ID3DX11EffectScalarVariable* inMenu;
};

//==============================================================================
// Sky effect
//==============================================================================
class SkyEffect : public Effect
{
public:
	SkyEffect(ID3D11Device* device, const std::wstring& filename);
	~SkyEffect();

	void SetWorldViewProj(CXMMATRIX m) { worldViewProj->SetMatrix(reinterpret_cast<const float*>(&m)); }
	void SetInMenu(bool menu) { inMenu->SetBool(menu); }
	void SetCubeMap(ID3D11ShaderResourceView* _cubeMap) { cubeMap->SetResource(_cubeMap); }

	ID3DX11EffectTechnique* skyTech;
	ID3DX11EffectScalarVariable  *inMenu;
	ID3DX11EffectMatrixVariable* worldViewProj;

	ID3DX11EffectShaderResourceVariable* cubeMap;
};

//==============================================================================
// Effect instances
//==============================================================================
class Effects
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static BasicEffect* BasicFX;
	static NormalMapEffect* NormalMapFX;
	static SkyEffect* SkyFX;
};

#endif