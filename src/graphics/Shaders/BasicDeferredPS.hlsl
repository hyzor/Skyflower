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
	float2 Velocity : SV_Target3;
};

PixelOut main(VertexOut pIn)
{
	PixelOut pOut;

	if (type == 0) //textured
	{
		// Sample color from texture
		pOut.Color = gDiffuseMap.Sample(samAnisotropic, pIn.Tex);

		pOut.Normal = float4(pIn.NormalW, 0.0f);

		pOut.Specular = gMaterial.Specular;

		// Bake shadow factor into color w component
		float shadowFactor = CalcShadowFactor(samShadow, gShadowMap, pIn.ShadowPosH);

		pOut.Color.w = shadowFactor;
	}
	else if (type == 1) //cloud
	{
		pOut.Color = gMaterial.Diffuse;

		pOut.Normal = float4(pIn.NormalW, 1.0f);

		pOut.Specular = gMaterial.Specular;


		pOut.Color.w = 1.0f;

	}
	else if (type == 2) //no texture
	{
		pOut.Color = gMaterial.Diffuse;

		pOut.Normal = float4(pIn.NormalW, 0.0f);

		pOut.Specular = gMaterial.Specular;

		// Bake shadow factor into color w component
		float shadowFactor = CalcShadowFactor(samShadow, gShadowMap, pIn.ShadowPosH);

		pOut.Color.w = shadowFactor;
	}

	
	

	

	//pOut.Position = float4(pIn.PosW, 1.0f);

	
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
