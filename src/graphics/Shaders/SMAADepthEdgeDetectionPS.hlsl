/**
* Color Edge Detection
*
* IMPORTANT NOTICE: color edge detection requires gamma-corrected colors, and
* thus 'colorTex' should be a non-sRGB texture.
*/

#include "SMAA/SMAA_Shared.hlsli"

cbuffer cbPerFrame : register(b0)
{
	unsigned int screenWidth;
	unsigned int screenHeight;
	float2 padding;
};

Texture2D depthTex : register(t0);

struct VertexIn
{
	float2 texCoord : TEXCOORD;
	float4 offset[3] : OFFSET;
};

float2 main(VertexIn vIn) : SV_TARGET
{
	float3 neighbours = SMAAGatherNeighbours(vIn.texCoord, vIn.offset, SMAATexturePass2D(depthTex), screenWidth, screenHeight);
	float2 delta = abs(neighbours.xx - float2(neighbours.y, neighbours.z));
	float2 edges = step(SMAA_DEPTH_THRESHOLD, delta);

	if (dot(edges, float2(1.0, 1.0)) == 0.0)
		discard;

	return edges;
}