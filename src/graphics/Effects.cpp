//------------------------------------------------------------------------------------
// File: Effects.cp
//
// Encapsulates all the effects (.fx files)
//------------------------------------------------------------------------------------

#include "Effects.h"

//==============================================================================
// Abstract effect
//==============================================================================
Effect::Effect(ID3D11Device* device, const std::wstring& filename)
{
	mFX = 0;
	std::ifstream fin(filename, std::ios::binary);
	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();

	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size,
		0, device, &mFX));
}

Effect::~Effect()
{
	ReleaseCOM(mFX);
}

//==============================================================================
// Basic effect
//==============================================================================
BasicEffect::BasicEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	//---------------------------------------------------------------
	// Techniques
	//---------------------------------------------------------------
	PointLight1Tech = mFX->GetTechniqueByName("PointLight1");
	DirLights3Tech = mFX->GetTechniqueByName("DirLights3");

	PointLight1TexTech = mFX->GetTechniqueByName("PointLight1Tex");
	DirLights3TexTech = mFX->GetTechniqueByName("DirLights3Tex");

	DirLights3TexAlphaClipTech = mFX->GetTechniqueByName("DirLights3TexAlphaClip");

	DirLights3FogTech = mFX->GetTechniqueByName("DirLights3Fog");

	DirLights3FogTexTech = mFX->GetTechniqueByName("DirLights3FogTex");

	DirLights3FogReflectionTech = mFX->GetTechniqueByName("DirLights3FogReflection");

	//-------------

	WorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	WorldViewProjTex  = mFX->GetVariableByName("gWorldViewProjTex")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform      = mFX->GetVariableByName("gTexTransform")->AsMatrix();

	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();

	PointLight = mFX->GetVariableByName("gPointLight");
	DirLights = mFX->GetVariableByName("gDirLights");

	ShadowTransform = mFX->GetVariableByName("gShadowTransform")->AsMatrix();
	ShadowMap = mFX->GetVariableByName("gShadowMap")->AsShaderResource();

	Mat               = mFX->GetVariableByName("gMaterial");
	DiffuseMap        = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	CubeMap = mFX->GetVariableByName("gCubeMap")->AsShaderResource();

	FogStart = mFX->GetVariableByName("gFogStart")->AsScalar();
	FogRange = mFX->GetVariableByName("gFogRange")->AsScalar();
	FogColor = mFX->GetVariableByName("gFogColor")->AsVector();
}

BasicEffect::~BasicEffect()
{

}

//==============================================================================
// Normal effect
//==============================================================================
NormalMapEffect::NormalMapEffect(ID3D11Device* device, const std::wstring& filename)
: Effect(device, filename)
{
	//---------------------------------------------------------------
	// Techniques
	//---------------------------------------------------------------
	DirLights3TexTech = mFX->GetTechniqueByName("DirLights3Tex");
	DirLights3TexAlphaClipTech = mFX->GetTechniqueByName("DirLights3TexAlphaClip");
	DirLights3TexSkinnedTech = mFX->GetTechniqueByName("DirLights3TexSkinned");

	DirLights3PointLights12TexAlphaClipTech = mFX->GetTechniqueByName("DirLights3PointLights12TexAlphaClip");

	DirLights3PointLights12TexAlphaClipSkinnedTech = mFX->GetTechniqueByName("DirLights3PointLights12TexAlphaClipSkinned");

	//-------------

	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	WorldViewProjTex = mFX->GetVariableByName("gWorldViewProjTex")->AsMatrix();
	World = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform = mFX->GetVariableByName("gTexTransform")->AsMatrix();

	inMenu = mFX->GetVariableByName("inMenu")->AsScalar();

	EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();

	dirLights = mFX->GetVariableByName("gDirLights");
	PointLights = mFX->GetVariableByName("gPointLights");

	shadowTransform = mFX->GetVariableByName("gShadowTransform")->AsMatrix();
	shadowMap = mFX->GetVariableByName("gShadowMap")->AsShaderResource();

	Mat = mFX->GetVariableByName("gMaterial");
	DiffuseMap = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	CubeMap = mFX->GetVariableByName("gCubeMap")->AsShaderResource();

	NormalMap = mFX->GetVariableByName("gNormalMap")->AsShaderResource();

	fogStart = mFX->GetVariableByName("gFogStart")->AsScalar();
	fogRange = mFX->GetVariableByName("gFogRange")->AsScalar();
	fogColor = mFX->GetVariableByName("gFogColor")->AsVector();

	BoneTransforms = mFX->GetVariableByName("gBoneTransforms")->AsMatrix();
}

NormalMapEffect::~NormalMapEffect()
{

}

BasicEffect* Effects::BasicFX = 0;
NormalMapEffect* Effects::NormalMapFX = 0;

void Effects::InitAll(ID3D11Device* device)
{
	BasicFX = new BasicEffect(device, L"Data/FX/Basic.fxo2013");
	NormalMapFX = new NormalMapEffect(device, L"Data/FX/NormalMap.fxo2013");
}

void Effects::DestroyAll()
{
	SafeDelete(BasicFX);
	SafeDelete(NormalMapFX);
}