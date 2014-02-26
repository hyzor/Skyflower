#include "BasicDeferredVS.hlsl"
#include "LightDef.hlsli"

cbuffer cPerObject : register(b0)
{
	Material gMaterial;
	unsigned int gGlobalMaterialIndex;
	int type;
	float2 padding;

	float4 gNearDepths;
	float4 gFarDepths;
	int gNrOfCascades;
	float3 padding1;
}

Texture2D gDiffuseMap : register(t0);
Texture2D gShadowMap1 : register(t1);
Texture2D gShadowMap2 : register(t2);
Texture2D gShadowMap3 : register(t3);
//TODO: Texture2DArray gShadowMaps : register(t2);

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

	pOut.Color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	pOut.Normal = float4(0.0f, 0.0f, 0.0f, 0.0f);
	//pOut.Specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	pOut.Velocity = float2(0.0f, 0.0f);

	float shadowFactor = 1.0f;

	if (type == 0 || type == 2)
	{
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

	}

	// Textured
	if (type == 0)
	{
		// Sample color from texture
		pOut.Color = gDiffuseMap.Sample(samAnisotropic, pIn.Tex);

		pOut.Normal = float4(pIn.NormalW, 0.0f);

		//pOut.Specular = gMaterial.Specular;

		// Bake shadow factor into color w component
		//float shadowFactor = CalcShadowFactor(samShadow, gShadowMap, pIn.ShadowPosH);

		//pOut.Color.w = shadowFactor;
		pOut.Normal.w = shadowFactor;
	}

	// Cloud
	else if (type == 1)
	{
		pOut.Color = gMaterial.Diffuse;

		pOut.Normal = float4(pIn.NormalW, 1.0f);

		//pOut.Specular = gMaterial.Specular;

		//pOut.Color.w = 1.0f;
		pOut.Normal.w = shadowFactor;
	}

	// No texture
	else if (type == 2)
	{
		pOut.Color = gMaterial.Diffuse;

		pOut.Normal = float4(pIn.NormalW, 0.0f);

		//pOut.Specular = gMaterial.Specular;

		// Bake shadow factor into color w component
		//float shadowFactor = CalcShadowFactor(samShadow, gShadowMap, pIn.ShadowPosH);

		//pOut.Color.w = shadowFactor;
		pOut.Normal.w = shadowFactor;
	}
	else
	{
		pOut.Color = gMaterial.Diffuse;

		pOut.Normal = float4(pIn.NormalW, 0.0f);

		pOut.Normal.w = shadowFactor;
	}

	//pOut.Position = float4(pIn.PosW, 1.0f);

	// Output material index
	//float globalMat = gGlobalMaterialIndex;
	//pOut.Normal.w = globalMat / 255.0f;
	pOut.Normal.w = shadowFactor;

	pOut.Color.w = (float)gGlobalMaterialIndex / 255.0f;
	//pOut.Color.w = shadowFactor;
	
	// Gamma correct color (make it linear)
	pOut.Color.xyz = pow(pOut.Color.xyz, 2.2f);

	//pOut.Velocity = ((pIn.CurPosH - pIn.PrevPosH) / 2.0f).xy;
	//pOut.Velocity = pIn.Velocity;

	//pIn.CurPosH = pIn.CurPosH * 0.5f + 0.5f;
	//pIn.PrevPosH = pIn.PrevPosH * 0.5f + 0.5f;

	float2 CurPosXY;
	float2 PrevPosXY;

	CurPosXY = (pIn.CurPosH.xy / pIn.CurPosH.w) * 0.5f + 0.5f;
	PrevPosXY = (pIn.PrevPosH.xy / pIn.PrevPosH.w) * 0.5f + 0.5f;

	pOut.Velocity = (CurPosXY - PrevPosXY) * 0.5f + 0.5f;
	pOut.Velocity = pow(pOut.Velocity, 3.0f);

	return pOut;
}
