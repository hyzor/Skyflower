#include "DrawParticleGS.hlsl"

Texture2DArray gTexArray : register(t0);

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

	pOut.Color = gTexArray.Sample(samLinear, float3(pIn.Tex, 0)) * pIn.Color;

	// Cast no shadow
	pOut.Color.w = 1.0f;

	// Dunno
	pOut.Normal = float4(0.0f, 0.0f, 0.0f, 0.0f);

	pOut.Specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	return pOut;
}