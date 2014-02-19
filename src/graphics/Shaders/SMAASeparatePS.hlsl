#include "SMAA/SMAA_Shared.hlsli"

Texture2DMS<float4, 2> colorTexMS : register(t0);

struct VertexOut
{
	float4 position : POSITION;
	float2 texCoord : TEXCOORD;
};

struct PixelOut
{
	float4 target0 : SV_Target0;
	float4 target1 : SV_Target1;
};

PixelOut main(VertexOut pIn)
{
	PixelOut pOut;

	int2 pos = int2(pIn.position.xy);
	pOut.target0 = SMAALoad(colorTexMS, pos, 0);
	pOut.target1 = SMAALoad(colorTexMS, pos, 1);

	return pOut;
}