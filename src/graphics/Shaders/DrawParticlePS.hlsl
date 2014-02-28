#include "DrawParticleGS.hlsl"
#include "Shared.hlsli"

Texture2DArray gTexArray : register(t0);

Texture2D gLitScene : register(t1);
Texture2D gSceneDepth : register(t2);

SamplerState samLinear : register(s0);
SamplerState samPoint : register(s1);

struct PixelOut
{
	float4 Color : SV_Target0;
	float4 Normal : SV_Target1;
	//float4 Specular : SV_Target2;
	float2 Velocity : SV_Target2;
	float4 Background : SV_Target3;
};

PixelOut main(GeoOut pIn)
{
	PixelOut pOut;

	// Clip pixels in the texture that are alpha 0.1f or lower
	// The z-component in float3 represents the texture index in the texture array
	float alpha = gTexArray.Sample(samLinear, float3(pIn.Tex, pIn.TexIndex)).a;
	clip(alpha - 0.1f);

	// Dont bother drawing pixel if alpha is so minimal it's fully transparent
	clip(pIn.Color.w - 0.001f);

	float4 sceneColor = gLitScene.Sample(samPoint, pIn.TexSpace);

	pOut.Color = gTexArray.Sample(samLinear, float3(pIn.Tex, pIn.TexIndex)) * pIn.Color;

	// Alpha blending
	if (pIn.BlendingMethod == ALPHA_BLENDING)
	{
		float realAlpha = pIn.Color.w - (1.0f - alpha);

		float sceneDepth = gSceneDepth.Sample(samPoint, pIn.TexSpace).x;

		sceneDepth = (pIn.zFar * pIn.zNear) / (pIn.zFar - sceneDepth * (pIn.zFar - pIn.zNear));

		float zDiff = sceneDepth - pIn.Depth;

		// TODO: Send in SoftParticleContrast and SoftParticleScale
		// These are tweakable and should be in the range of 1.0f to 5.0f and 0.0f to 5.0f respectively
		float SoftParticleContrast = 2.5f;
		float SoftParticleScale = 0.15f;

		float contrast = Contrast(zDiff * SoftParticleScale, SoftParticleContrast);

		// If contrast * zDiff is negative, it means this particle is behind some opaque object
		//if (contrast * zDiff <= 0.0f)
		//discard;

		realAlpha = realAlpha * contrast;

		if (realAlpha < 0.0f)
			realAlpha = 0.0f;

		clip(realAlpha - 0.001f);

		float3 alphaFloat3 = float3(realAlpha, realAlpha, realAlpha);
		float3 alphaFloat3Inv = float3(1.0f - realAlpha, 1.0f - realAlpha, 1.0f - realAlpha);

		float3 colorOut = pOut.Color.xyz * alphaFloat3 + sceneColor.xyz * alphaFloat3Inv;
		pOut.Background.xyz = colorOut;

		// Avoid using additive blending in light accumulation stage
		// by always making sure the alpha value isn't 1.0f
		if (realAlpha < 1.0f)
			pOut.Background.w = realAlpha;
		else
			pOut.Background.w = 0.999f;
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

	pOut.Color.w = -1.0f;

	// This will set the light accumulation shader to use a default material
	//pOut.Color.w = 1000.0f;

	pOut.Normal.xyz = pIn.NormalW;

	// No shadow cast on it
	pOut.Normal.w = 1.0f;

	//pOut.Specular = float4(0.0f, 0.0f, 0.0f, 1.0f);

	// Gamma correct color (make it linear)
	pOut.Color.xyz = pow(pOut.Color.xyz, 2.2f);
	pOut.Background.xyz = pow(pOut.Background.xyz, 2.2f);

	float2 CurPosXY;
	float2 PrevPosXY;

	CurPosXY = (pIn.CurPosH.xy / pIn.CurPosH.w) * 0.5f + 0.5f;
	PrevPosXY = (pIn.PrevPosH.xy / pIn.PrevPosH.w) * 0.5f + 0.5f;

	pOut.Velocity = (CurPosXY - PrevPosXY) * 0.5f + 0.5f;
	pOut.Velocity = pow(pOut.Velocity, 3.0f);

	return pOut;
}