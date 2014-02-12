#include "SMAA/SMAA_Shared.hlsli"

cbuffer cbPerFrame : register(b0)
{
	unsigned int screenHeight;
	unsigned int screenWidth;
	float2 padding;
};

struct VertexIn
{
	float2 texCoord : TEXCOORD;
};

struct VertexOut
{
	float2 pixCoord : PIXCOORD;
	float4 offset[3] : OFFSET;
};

VertexOut main(VertexIn vIn)
{
	VertexOut vOut;

	float4 SMAA_RT_METRICS = float4(1.0 / screenWidth, 1.0 / screenHeight, screenWidth, screenHeight);

	vOut.pixCoord = vIn.texCoord * SMAA_RT_METRICS.zw;

	// We will use these offsets for the searches later on (see @PSEUDO_GATHER4):
	vOut.offset[0] = mad(SMAA_RT_METRICS.xyxy, float4(-0.25, -0.125, 1.25, -0.125), vIn.texCoord.xyxy);
	vOut.offset[1] = mad(SMAA_RT_METRICS.xyxy, float4(-0.125, -0.25, -0.125, 1.25), vIn.texCoord.xyxy);

	// And these for the searches, they indicate the ends of the loops:
	vOut.offset[2] = mad(SMAA_RT_METRICS.xxyy,
	float4(-2.0, 2.0, -2.0, 2.0) * float(SMAA_MAX_SEARCH_STEPS),
	float4(vOut.offset[0].xz, vOut.offset[1].yw));

	return vOut;
}