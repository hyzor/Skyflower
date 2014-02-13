#include "SMAA/SMAA_Shared.hlsli"

cbuffer cbPerFrame : register(b0)
{
	unsigned int screenWidth;
	unsigned int screenHeight;
	float2 padding;
};

struct VertexIn
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD;
};

struct VertexOut
{
	float2 texCoord : TEXCOORD;
	float4 offset : OFFSET;
};

VertexOut main(VertexIn vIn)
{
	VertexOut vOut;

	float4 SMAA_RT_METRICS = float4(1.0 / screenWidth, 1.0 / screenHeight, screenWidth, screenHeight);

	vOut.offset = mad(SMAA_RT_METRICS.xyxy, float4(1.0, 0.0, 0.0, 1.0), vIn.texCoord.xyxy);

	vOut.texCoord = vIn.texCoord;

	return vOut;
}