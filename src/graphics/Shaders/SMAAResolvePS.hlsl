#include "SMAA/SMAA_Shared.hlsli"

Texture2D currentColorTex : register(t0);
Texture2D previousColorTex : register(t1);
Texture2D velocityTex : register(t2);

struct VertexOut
{
	float2 texCoord : TEXCOORD;
};

SamplerState samPoint : register(s0);

float4 main(VertexOut pIn) : SV_TARGET
{
#if SMAA_REPROJECTION
	// Velocity is assumed to be calculated for motion blur, so we need to
	// inverse it for reprojection:
	float2 velocity = -SMAA_DECODE_VELOCITY(SMAASamplePoint(velocityTex, pIn.texCoord).rg);

		// Fetch current pixel:
		float4 current = SMAASamplePoint(currentColorTex, pIn.texCoord, samPoint);

		// Reproject current coordinates and fetch previous pixel:
		float4 previous = SMAASamplePoint(previousColorTex, pIn.texCoord + velocity, samPoint);

		// Attenuate the previous pixel if the velocity is different:
		float delta = abs(current.a * current.a - previous.a * previous.a) / 5.0;
	float weight = 0.5 * saturate(1.0 - sqrt(delta) * SMAA_REPROJECTION_WEIGHT_SCALE);

	// Blend the pixels according to the calculated weight:
	return lerp(current, previous, weight);
#else
	// Just blend the pixels:
	float4 current = SMAASamplePoint(currentColorTex, pIn.texCoord, samPoint);
		float4 previous = SMAASamplePoint(previousColorTex, pIn.texCoord, samPoint);
	return lerp(current, previous, 0.5);
#endif
}