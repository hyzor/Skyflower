#include "BasicDeferredVS.hlsl"
#include "LightDef.hlsli"

cbuffer cPerObject : register(b0)
{
	Material gMaterial;
	int type;
	float3 skit;
}

Texture2D gDiffuseMap : register(t0);
Texture2D gShadowMap : register(t1);

SamplerState samLinear : register(s0);
SamplerState samAnisotropic : register(s1);
SamplerComparisonState samShadow : register(s2);

struct PixelOut
{
	float4 Color : SV_Target0;
	float4 Normal : SV_Target1;
	float4 Specular : SV_Target2;
	//float4 Position : SV_Target3;
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

	// Bake shadow factor into color w component
	float shadowFactor = CalcShadowFactor(samShadow, gShadowMap, pIn.ShadowPosH);

	if (type == 1)
		pOut.Color.w = 1.0f;
	else
		pOut.Color.w = shadowFactor;

	return pOut;
}