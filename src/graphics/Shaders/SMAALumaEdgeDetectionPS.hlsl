/**
* Luma Edge Detection
*
* IMPORTANT NOTICE: luma edge detection requires gamma-corrected colors, and
* thus 'colorTex' should be a non-sRGB texture.
*/

#include "SMAA/SMAA_Shared.hlsli"

Texture2D colorTex : register(t0);
Texture2D predicationTex : register(t1);

struct VertexIn
{
	float2 texCoord : TEXCOORD;
	float4 offset[3] : OFFSET;
};

float2 main(VertexIn vIn) : SV_TARGET
{
	// Calculate the threshold:
#if SMAA_PREDICATION
	float2 threshold = SMAACalculatePredicatedThreshold(texcoord, offset, SMAATexturePass2D(predicationTex));
#else
	float2 threshold = float2(SMAA_THRESHOLD, SMAA_THRESHOLD);
#endif

	// Calculate lumas:
	float3 weights = float3(0.2126, 0.7152, 0.0722);
	float L = dot(SMAASamplePoint(colorTex, vIn.texCoord).rgb, weights);

	float Lleft = dot(SMAASamplePoint(colorTex, vIn.offset[0].xy).rgb, weights);
	float Ltop = dot(SMAASamplePoint(colorTex, vIn.offset[0].zw).rgb, weights);

	// We do the usual threshold:
	float4 delta;
	delta.xy = abs(L - float2(Lleft, Ltop));
	float2 edges = step(threshold, delta.xy);

	// Then discard if there is no edge:
	if (dot(edges, float2(1.0, 1.0)) == 0.0)
		discard;

	// Calculate right and bottom deltas:
	float Lright = dot(SMAASamplePoint(colorTex, vIn.offset[1].xy).rgb, weights);
	float Lbottom = dot(SMAASamplePoint(colorTex, vIn.offset[1].zw).rgb, weights);
	delta.zw = abs(L - float2(Lright, Lbottom));

	// Calculate the maximum delta in the direct neighborhood:
	float2 maxDelta = max(delta.xy, delta.zw);

	// Calculate left-left and top-top deltas:
	float Lleftleft = dot(SMAASamplePoint(colorTex, vIn.offset[2].xy).rgb, weights);
	float Ltoptop = dot(SMAASamplePoint(colorTex, vIn.offset[2].zw).rgb, weights);
	delta.zw = abs(float2(Lleft, Ltop) - float2(Lleftleft, Ltoptop));

	// Calculate the final maximum delta:
	maxDelta = max(maxDelta.xy, delta.zw);
	float finalDelta = max(maxDelta.x, maxDelta.y);

	// Local contrast adaptation:
	edges.xy *= step(finalDelta, SMAA_LOCAL_CONTRAST_ADAPTATION_FACTOR * delta.xy);

	return edges;
}