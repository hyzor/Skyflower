cbuffer cbPerObject : register(b0)
{
	float4x4 gLightWorldViewProj;
};
struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex : TEXCOORD;
};

struct VertexOut
{
	float4 Pos : SV_POSITION;
};

VertexOut main(VertexIn vIn)
{
	VertexOut vout;

	vout.Pos = mul(float4(vIn.PosL, 1.0f), gLightWorldViewProj);

	return vout;
}