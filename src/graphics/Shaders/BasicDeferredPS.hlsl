#include "BasicDeferredVS.hlsl"
#include "LightDef.hlsli"

cbuffer cPerObject : register(b0)
{
	//float nrOfCascades;
	//float d, e, f;

	Material gMaterial;
	float type;
	//float3 skit; //B-E-A-UTIFUL!
	float a, b, c;

	float4 gNearDepths;
	float4 gFarDepths;

	//float3 ytterligareSkit; // (Padding)
}

Texture2D gDiffuseMap : register(t0);
Texture2D gShadowMap : register(t1);
Texture2D gShadowMap1 : register(t2);
Texture2D gShadowMap2 : register(t3);
Texture2D gShadowMap3 : register(t4);
//TODO: Texture2DArray gShadowMaps : register(t2);

SamplerState samLinear : register(s0);
SamplerState samAnisotropic : register(s1);
SamplerComparisonState samShadow : register(s2);

struct PixelOut
{
	float4 Color : SV_Target0;
	float4 Normal : SV_Target1;
	float4 Specular : SV_Target2;
	//float4 Position : SV_Target3;
	float2 Velocity : SV_Target3;
};

PixelOut main(VertexOut pIn)
{
	PixelOut pOut;

	// Sample color from texture
	if(type == 0)
		pOut.Color = gDiffuseMap.Sample(samAnisotropic, pIn.Tex);
	else
		pOut.Color = gMaterial.Diffuse;

	if(type == 1)
		pOut.Normal = float4(pIn.NormalW, 1.0f);
	else
		pOut.Normal = float4(pIn.NormalW, 0.0f);

	pOut.Specular = gMaterial.Specular;

	//pOut.Position = float4(pIn.PosW, 1.0f);

	int cascadeIndex = 0;
	float shadowFactor = 0.0f;

	if (true)
	{

		if (pIn.Depth > gFarDepths.z)
			shadowFactor = 1.0f;

		//Compare the depth of current pixel in camera space and compare to given near and far depths
		//to decide appropriate index of cascade to sample from
		//for (int i = 0; i < nrOfCascades; i++)
		//{
		//	if (pIn.Depth < gFarDepths[i] && pIn.Depth > gNearDepths[i])
		//	{
		//		cascadeIndex = i;
		//	}
		//}

		if (pIn.Depth > gNearDepths.x && pIn.Depth < gFarDepths.x)
			cascadeIndex = 0;
		else if (pIn.Depth > gNearDepths.y && pIn.Depth < gFarDepths.y)
			cascadeIndex = 1;
		else if (pIn.Depth > gNearDepths.z && pIn.Depth < gFarDepths.z)
			cascadeIndex = 2;

		//shadowPos = mul(pIn.TexShadow, gShadowProjTex[cascadeIndex]); //Transform to shadow projection texture space

		if (cascadeIndex == 0)
			shadowFactor = CalcShadowFactor(samShadow, gShadowMap1, pIn.ShadowPosH1); //Cascade 1
		else if (cascadeIndex == 1)
			shadowFactor = CalcShadowFactor(samShadow, gShadowMap2, pIn.ShadowPosH2); //Cascade 2
		else if (cascadeIndex == 2)
			shadowFactor = CalcShadowFactor(samShadow, gShadowMap3, pIn.ShadowPosH3); //Cascade 3

	}
	// Bake shadow factor into color w component
	//shadowFactor = CalcShadowFactor(samShadow, gShadowMap, pIn.ShadowPosH);

	if (type == 1)
		pOut.Color.w = 1.0f;
	else
		pOut.Color.w = shadowFactor;

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