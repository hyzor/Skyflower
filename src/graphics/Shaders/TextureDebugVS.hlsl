cbuffer cbPerFrame : register(c0)
{
	float4x4 gWorldViewProj;
}

Texture2D gTexture;

struct VertexIn
{
	float3 PosL : POSITION;
};

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}