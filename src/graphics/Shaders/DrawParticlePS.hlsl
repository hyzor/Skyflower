#include "DrawParticleGS.hlsl"
#include "Shared.hlsli"

Texture2DArray gTexArray : register(t0);

Texture2D gLitScene : register(t1);

SamplerState samLinear : register(s0);
SamplerState samPoint : register(s1);

struct PixelOut
{
	float4 Color : SV_Target0;
	float4 Normal : SV_Target1;
	float4 Specular : SV_Target2;
	float2 Velocity : SV_Target3;
	float4 Background : SV_Target4;
};

PixelOut main(GeoOut pIn)
{
	PixelOut pOut;

	// Clip pixels in the texture that are alpha 0.1f or lower
	// The z-component in float3 represents the texture index in the texture array
	float alpha = gTexArray.Sample(samLinear, float3(pIn.Tex, pIn.TexIndex) * pIn.Color.xyz).a;
	clip(alpha - 0.1f);

	// Dont bother drawing pixel if alpha is so minimal it's fully transparent
	//clip(pIn.Color.w - 0.001f);

	float4 sceneColor = gLitScene.Sample(samPoint, pIn.TexSpace);

	pOut.Color = gTexArray.Sample(samLinear, float3(pIn.Tex, pIn.TexIndex)) * pIn.Color;

	float realAlpha = pIn.Color.w - (1.0f - alpha);

	if (realAlpha < 0.0f)
		realAlpha = 0.0f;

	// Alpha blending
	if (pIn.BlendingMethod == ALPHA_BLENDING)
	{
		float3 alphaFloat3 = float3(realAlpha, realAlpha, realAlpha);
		float3 alphaFloat3Inv = float3(1.0f - realAlpha, 1.0f - realAlpha, 1.0f - realAlpha);

			float3 colorOut = pOut.Color.xyz * alphaFloat3 + sceneColor.xyz * alphaFloat3Inv;

			//pOut.Color.xyz = colorOut.xyz;

			//pOut.Background.xyz = float3(0.0f, 0.0f, 0.0f);
			//pOut.Background.xyz = sceneColor.xyz;
			pOut.Background.xyz = colorOut;
			pOut.Background.w = realAlpha;

			// Fill the diffuse buffer with black color
			//pOut.Color.xyz = float3(0.0f, 0.0f, 0.0f);
	}

	// Additive blending
	else if (pIn.BlendingMethod == ADDITIVE_BLENDING)
	{
		float3 colorOut = pOut.Color.xyz * float3(1.0f, 1.0f, 1.0f) + sceneColor.xyz * float3(1.0f, 1.0f, 1.0f);

		pOut.Background.xyz = colorOut.xyz;
		pOut.Background.w = 1.0f;

		// Fill the diffuse buffer with black color
		pOut.Color.xyz = float3(0.0f, 0.0f, 0.0f);
	}

	// No blending
	else
	{
		pOut.Background = float4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	// No shadow cast on it
	pOut.Color.w = 1.0f;

	// Not affected by lights
	//pOut.Normal = float4(0.0f, 1.0f, 0.0f, 0.0f);
	pOut.Normal.xyz = pIn.NormalW;
	pOut.Normal.w = 0.0f;

	pOut.Specular = float4(0.0f, 0.0f, 0.0f, 1.0f);

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