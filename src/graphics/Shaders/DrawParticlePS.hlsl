#include "DrawParticleGS.hlsl"

Texture2DArray gTexArray : register(t0);

Texture2D gLitScene : register(t1);

SamplerState samLinear : register(s0);

struct PixelOut
{
	float4 Color : SV_Target0;
	float4 Normal : SV_Target1;
	float4 Specular : SV_Target2;
};

PixelOut main(GeoOut pIn)
{
	PixelOut pOut;

	// Clip pixels that are alpha 0.15f or lower
	// The z-component in float3 represents the texture index in the texture array
	float alpha = gTexArray.Sample(samLinear, float3(pIn.Tex, pIn.TexIndex) * pIn.Color).a;

	clip(alpha - 0.15f);

	float4 sceneColor = gLitScene.Sample(samLinear, pIn.Tex);


	pOut.Color = gTexArray.Sample(samLinear, float3(pIn.Tex, pIn.TexIndex)) * pIn.Color;

	float3 colorOut = lerp(pOut.Color.xyz, sceneColor.xyz, pOut.Color.w);

	pOut.Color.xyz = colorOut.xyz;

	// No shadow cast on it
	pOut.Color.w = 1.0f;

	// Not affected by lights
	pOut.Normal = float4(0.0f, 0.0f, 0.0f, 1.0f);

	pOut.Specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	return pOut;
}