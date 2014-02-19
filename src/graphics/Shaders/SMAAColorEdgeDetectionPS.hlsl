/**
* Color Edge Detection
*
* IMPORTANT NOTICE: color edge detection requires gamma-corrected colors, and
* thus 'colorTex' should be a non-sRGB texture.
*/

#include "SMAA/SMAA_Shared.hlsli"
#include "SMAAEdgeDetectionVS.hlsl"

Texture2D colorTex : register(t0);
Texture2D predicationTex : register(t1);

SamplerState samPoint : register(s0);

/*
struct VertexIn
{
	float2 texCoord : TEXCOORD;
	float4 offset[3] : OFFSET;
};
*/

float2 main(VertexOut vIn) : SV_TARGET
{
	// Calculate the threshold:
#if SMAA_PREDICATION
	float2 threshold = SMAACalculatePredicatedThreshold(texcoord, offset, predicationTex);
#else
	float2 threshold = float2(SMAA_THRESHOLD, SMAA_THRESHOLD);
#endif

	// Calculate color deltas:
	float4 delta;
	float3 C = SMAASamplePoint(colorTex, vIn.texCoord, samPoint).rgb;

	float3 Cleft = SMAASamplePoint(colorTex, vIn.offset[0].xy, samPoint).rgb;
	float3 t = abs(C - Cleft);
	delta.x = max(max(t.r, t.g), t.b);

	float3 Ctop = SMAASamplePoint(colorTex, vIn.offset[0].zw, samPoint).rgb;
	t = abs(C - Ctop);
	delta.y = max(max(t.r, t.g), t.b);

	// We do the usual threshold:
	float2 edges = step(threshold, delta.xy);

	// Then discard if there is no edge:
	if (dot(edges, float2(1.0, 1.0)) == 0.0)
		discard;

	// Calculate right and bottom deltas:
	float3 Cright = SMAASamplePoint(colorTex, vIn.offset[1].xy, samPoint).rgb;
	t = abs(C - Cright);
	delta.z = max(max(t.r, t.g), t.b);

	float3 Cbottom = SMAASamplePoint(colorTex, vIn.offset[1].zw, samPoint).rgb;
	t = abs(C - Cbottom);
	delta.w = max(max(t.r, t.g), t.b);

	// Calculate the maximum delta in the direct neighborhood:
	float2 maxDelta = max(delta.xy, delta.zw);

	// Calculate left-left and top-top deltas:
	float3 Cleftleft = SMAASamplePoint(colorTex, vIn.offset[2].xy, samPoint).rgb;
	t = abs(C - Cleftleft);
	delta.z = max(max(t.r, t.g), t.b);

	float3 Ctoptop = SMAASamplePoint(colorTex, vIn.offset[2].zw, samPoint).rgb;
	t = abs(C - Ctoptop);
	delta.w = max(max(t.r, t.g), t.b);

	// Calculate the final maximum delta:
	maxDelta = max(maxDelta.xy, delta.zw);
	float finalDelta = max(maxDelta.x, maxDelta.y);

	// Local contrast adaptation:
	edges.xy *= step(finalDelta, SMAA_LOCAL_CONTRAST_ADAPTATION_FACTOR * delta.xy);

	return edges;
}