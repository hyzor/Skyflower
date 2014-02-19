/**
* Color Edge Detection
*
* IMPORTANT NOTICE: color edge detection requires gamma-corrected colors, and
* thus 'colorTex' should be a non-sRGB texture.
*/

#include "SMAA/SMAA_Shared.hlsli"
#include "SMAAEdgeDetectionVS.hlsl"

cbuffer cbPerFrame : register(b0)
{
	unsigned int ps_screenWidth;
	unsigned int ps_screenHeight;
	float2 ps_padding;
};

Texture2D depthTex : register(t0);

SamplerState samLinear : register(s0);

/*
struct VertexIn
{
	float2 texCoord : TEXCOORD;
	float4 offset[3] : OFFSET;
};
*/

float2 main(VertexOut vIn) : SV_TARGET
{
	float4 SMAA_RT_METRICS = float4(1.0 / ps_screenWidth, 1.0 / ps_screenHeight, ps_screenWidth, ps_screenHeight);

	float3 neighbours = SMAAGatherNeighbours(vIn.texCoord, vIn.offset, SMAATexturePass2D(depthTex), screenWidth, samLinear);
	float2 delta = abs(neighbours.xx - float2(neighbours.y, neighbours.z));
	float2 edges = step(SMAA_DEPTH_THRESHOLD, delta);

	if (dot(edges, float2(1.0, 1.0)) == 0.0)
		discard;

	return edges;
}