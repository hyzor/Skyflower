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

	int cascadeIndex = 0;
	int nrOfCascades = gNrOfCascades;
	float depth = pIn.Depth;
	float4 shadowPosH1 = pIn.ShadowPosH1;
	float4 shadowPosH2 = pIn.ShadowPosH2;
	float4 shadowPosH3 = pIn.ShadowPosH3;

		//If current depth is beyond the furthest depth, dont shadow it
	if (depth > gFarDepths.x && nrOfCascades == 1)
		shadowFactor = 1.0f;
	if (depth > gFarDepths.y && nrOfCascades == 2)
		shadowFactor = 1.0f;
	if (depth > gFarDepths.z && nrOfCascades == 3)
		shadowFactor = 1.0f;

	//Compare the depth of current pixel in camera space to given near and far depths
	//to decide appropriate index of cascade to sample from
	if (depth > gNearDepths.x && depth < gFarDepths.x)
	{
		cascadeIndex = 0;
	}
	else if (depth > gNearDepths.y && depth < gFarDepths.y && nrOfCascades > 1)
	{
		cascadeIndex = 1;
	}
	else if (depth > gNearDepths.z && depth < gFarDepths.z && nrOfCascades > 2)
	{
		cascadeIndex = 2;
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

	pOut.Normal.w = shadowFactor;

	pOut.Color.w = gGlobalMaterialIndex / 1000.0f;

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