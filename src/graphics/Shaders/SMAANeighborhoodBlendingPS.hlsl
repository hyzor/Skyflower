#include "SMAA/SMAA_Shared.hlsli"

Texture2D colorTex : register(t0);
Texture2D blendTex : register(t1);
Texture2D velocityTex : register(t2);

cbuffer cbPerFrame : register(b0)
{
	unsigned int screenWidth;
	unsigned int screenHeight;
	float2 padding;
};

struct VertexIn
{
	float2 texCoord : TEXCOORD;
	float4 offset : OFFSET;
};

float4 main(VertexIn vIn) : SV_TARGET
{
	float4 SMAA_RT_METRICS = float4(1.0 / screenWidth, 1.0 / screenHeight, screenWidth, screenHeight);

	// Fetch the blending weights for current pixel:
	float4 a;
	a.x = SMAASample(blendTex, vIn.offset.xy).a; // Right
	a.y = SMAASample(blendTex, vIn.offset.zw).g; // Top
	a.wz = SMAASample(blendTex, vIn.texCoord).xz; // Bottom / Left

	// Is there any blending weight with a value greater than 0.0?
	SMAA_BRANCH
		if (dot(a, float4(1.0, 1.0, 1.0, 1.0)) < 1e-5)
		{
			float4 color = SMAASampleLevelZero(colorTex, vIn.texCoord);

#if SMAA_REPROJECTION
				float2 velocity = SMAA_DECODE_VELOCITY(SMAASampleLevelZero(velocityTex, vIn.texCoord));

				// Pack velocity into the alpha channel:
				color.a = sqrt(5.0 * length(velocity));
#endif

			return color;
		}
		else
		{
			bool h = max(a.x, a.z) > max(a.y, a.w); // max(horizontal) > max(vertical)

			// Calculate the blending offsets:
			float4 blendingOffset = float4(0.0, a.y, 0.0, a.w);
			float2 blendingWeight = a.yw;
			SMAAMovc(bool4(h, h, h, h), blendingOffset, float4(a.x, 0.0, a.z, 0.0));
			SMAAMovc(bool2(h, h), blendingWeight, a.xz);
			blendingWeight /= dot(blendingWeight, float2(1.0, 1.0));

			// Calculate the texture coordinates:
			float4 blendingCoord = mad(blendingOffset, float4(SMAA_RT_METRICS.xy, -SMAA_RT_METRICS.xy), vIn.texCoord.xyxy);

				// We exploit bilinear filtering to mix current pixel with the chosen
				// neighbor:
				float4 color = blendingWeight.x * SMAASampleLevelZero(colorTex, blendingCoord.xy);
				color += blendingWeight.y * SMAASampleLevelZero(colorTex, blendingCoord.zw);

#if SMAA_REPROJECTION
			// Antialias velocity for proper reprojection in a later stage:
			float2 velocity = blendingWeight.x * SMAA_DECODE_VELOCITY(SMAASampleLevelZero(velocityTex, blendingCoord.xy));
			velocity += blendingWeight.y * SMAA_DECODE_VELOCITY(SMAASampleLevelZero(velocityTex, blendingCoord.zw));

			// Pack velocity into the alpha channel:
			color.a = sqrt(5.0 * length(velocity));
#endif

			return color;
		}
}