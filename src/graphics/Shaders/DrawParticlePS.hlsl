#include "DrawParticleGS.hlsl"

Texture2DArray gTexArray : register(t0);

Texture2D gLitScene : register(t1);

SamplerState samLinear : register(s0);
SamplerState samPoint : register(s1);

struct PixelOut
{
	float4 Color : SV_Target0;
	float4 Normal : SV_Target1;
	float4 Specular : SV_Target2;
};

PixelOut main(GeoOut pIn)
{
	PixelOut pOut;

	// Clip pixels that are alpha 0.1f or lower
	// The z-component in float3 represents the texture index in the texture array
	float alpha = gTexArray.Sample(samLinear, float3(pIn.Tex, pIn.TexIndex) * pIn.Color.xyz).a;

	clip(alpha - 0.1f);

	float4 sceneColor = gLitScene.Sample(samPoint, pIn.TexSpace);

	pOut.Color = gTexArray.Sample(samLinear, float3(pIn.Tex, pIn.TexIndex)) * pIn.Color;

	float3 alphaFloat3 = float3(pIn.Color.w, pIn.Color.w, pIn.Color.w);
	float3 alphaFloat3Inv = float3(1.0f - pIn.Color.w, 1.0f - pIn.Color.w, 1.0f - pIn.Color.w);

	// Additive blending
	//float3 colorOut = pOut.Color.xyz * float3(1.0f, 1.0f, 1.0f) + sceneColor.xyz * float3(1.0f, 1.0f, 1.0f);

	// Alpha blending
	float3 colorOut = pOut.Color.xyz * alphaFloat3 + sceneColor.xyz * alphaFloat3Inv;

	pOut.Color.xyz = colorOut.xyz;

	// No shadow cast on it
	pOut.Color.w = 1.0f;

	// Not affected by lights
	pOut.Normal = float4(0.0f, 0.0f, 0.0f, 1.0f);

	pOut.Specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	return pOut;
}