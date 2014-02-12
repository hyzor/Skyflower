#include "SMAA/SMAA_Shared.hlsli"

cbuffer cbPerFrame : register(b0)
{
	unsigned int screenHeight;
	unsigned int screenWidth;
};

struct VertexIn
{
	float2 texCoord : TEXCOORD;
};

struct VertexOut
{
	float4 offset[3] : OFFSET;
};

VertexOut main(VertexIn vIn)
{
	VertexOut vOut;

	float4 SMAA_RT_METRICS = float4(1.0 / screenWidth, 1.0 / screenHeight, screenWidth, screenHeight);

	vOut.offset[0] = mad(SMAA_RT_METRICS.xyxy, float4(-1.0, 0.0, 0.0, -1.0), vIn.texCoord.xyxy);
	vOut.offset[1] = mad(SMAA_RT_METRICS.xyxy, float4(1.0, 0.0, 0.0, 1.0), vIn.texCoord.xyxy);
	vOut.offset[2] = mad(SMAA_RT_METRICS.xyxy, float4(-2.0, 0.0, 0.0, -2.0), vIn.texCoord.xyxy);

	return vOut;
}