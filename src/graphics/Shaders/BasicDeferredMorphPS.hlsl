#include "BasicDeferredMorphVS.hlsl"
#include "LightDef.hlsli"

cbuffer cPerObject : register(b0)
{
	Material gMaterial;
	float4 gNearDepths;
	float4 gFarDepths;
	int gNrOfCascades;
	unsigned int gGlobalMaterialIndex;
	float2 padding;
}

Texture2D gDiffuseMap : register(t0);
Texture2D gShadowMap1 : register(t1);
Texture2D gShadowMap2 : register(t2);
Texture2D gShadowMap3 : register(t3);
Texture2D gShadowMap4 : register(t4);

SamplerState samLinear : register(s0);
SamplerState samAnisotropic : register(s1);
SamplerComparisonState samShadow : register(s2);

struct PixelOut
{
	float4 Color : SV_Target0;
	float4 Normal : SV_Target1;
	//float4 Specular : SV_Target2;
	//float4 Position : SV_Target3;
	float2 Velocity : SV_Target2;
};

PixelOut main(VertexOut pIn)
{
	PixelOut pOut;

	// Sample color from texture
	pOut.Color = gDiffuseMap.Sample(samAnisotropic, pIn.Tex);

	pOut.Normal = float4(pIn.NormalW, 0.0f);

	//pOut.Specular = gMaterial.Specular;

	//pOut.Position = float4(pIn.PosW, 1.0f);

	// Bake shadow factor into color w component
	float shadowFactor = 1.0f;
	float shadowFactorBlend = 1.0f;

	int cascadeIndex = 0;
	int nrOfCascades = gNrOfCascades;
	float depth = pIn.Depth;
	float4 shadowPosH1 = pIn.ShadowPosH1;
	float4 shadowPosH2 = pIn.ShadowPosH2;
	float4 shadowPosH3 = pIn.ShadowPosH3;
	float4 shadowPosH4 = pIn.ShadowPosH4;
	int blendIndex = -1;

	//Compare the depth of current pixel in camera space to given near and far depths
	//to decide appropriate index of cascade to sample from
	if (nrOfCascades == 1 || depth > gNearDepths.x && depth < gFarDepths.x)
	{
		cascadeIndex = 0;
		if ((gFarDepths.x - depth) < BLEND_LIMIT_1)
			blendIndex = 1;
	}
	if (nrOfCascades > 1 && (depth > gNearDepths.y && depth < gFarDepths.y))
	{
		cascadeIndex = 1;
		if ((depth - gNearDepths.x) < BLEND_LIMIT_1)
			blendIndex = 0;
		else if ((gFarDepths.y - depth) < BLEND_LIMIT_2)
			blendIndex = 2;
	}
	if (nrOfCascades > 2 && (depth > gNearDepths.z && depth < gFarDepths.z))
	{
		cascadeIndex = 2;
		if ((depth - gNearDepths.y) < BLEND_LIMIT_2)
			blendIndex = 1;
		else if ((gFarDepths.z - depth) < BLEND_LIMIT_3)
			blendIndex = 3;
	}
	if (nrOfCascades > 3 && (depth > gNearDepths.w && depth < gFarDepths.w))
	{
		cascadeIndex = 3;
		if ((depth - gNearDepths.z) < BLEND_LIMIT_3)
			blendIndex = 2;
	}

	if (cascadeIndex == 0)
	{
		shadowFactor = CalcShadowFactor(samShadow, gShadowMap1, shadowPosH1); //Cascade 1
	}
	else if (cascadeIndex == 1)
	{
		shadowFactor = CalcShadowFactor(samShadow, gShadowMap2, shadowPosH2); //Cascade 2
	}
	else if (cascadeIndex == 2)
	{
		shadowFactor = CalcShadowFactor(samShadow, gShadowMap3, shadowPosH3); //Cascade 3
	}
	else if (cascadeIndex == 3)
	{
		shadowFactor = CalcShadowFactor(samShadow, gShadowMap4, shadowPosH4); //Cascade 3
	}

	switch (blendIndex)
	{
	case 0:
		shadowFactorBlend = CalcShadowFactor(samShadow, gShadowMap1, shadowPosH1);
		break;
	case 1:
		shadowFactorBlend = CalcShadowFactor(samShadow, gShadowMap2, shadowPosH2);
		break;
	case 2:
		shadowFactorBlend = CalcShadowFactor(samShadow, gShadowMap3, shadowPosH3);
		break;
	case 3:
		shadowFactorBlend = CalcShadowFactor(samShadow, gShadowMap4, shadowPosH4);
		break;
	}

	if (blendIndex != -1)
	{
		shadowFactor = lerp(shadowFactor, shadowFactorBlend, 0.5f);
	}

	pOut.Normal.w = shadowFactor;

	pOut.Color.w = gGlobalMaterialIndex / 255.0f;

	// Gamma correct color (make it linear)
	pOut.Color.xyz = pow(pOut.Color.xyz, 2.2f);

	float2 CurPosXY;
	float2 PrevPosXY;

	CurPosXY = (pIn.CurPosH.xy / pIn.CurPosH.w) * 0.5f + 0.5f;
	PrevPosXY = (pIn.PrevPosH.xy / pIn.PrevPosH.w) * 0.5f + 0.5f;

	pOut.Velocity = (CurPosXY - PrevPosXY) * 0.5f + 0.5f;
	pOut.Velocity = pow(pOut.Velocity, 3.0f);

	return pOut;
}