cbuffer cbPerObject : register(b0)
{
	float4x4 transformation;
	float4x4 projection;
	float4 color;
};

struct VertexOut
{
	float4 position : SV_POSITION;
};

struct VertexIn
{
	float2 position : POSITION;
};

VertexOut main(VertexIn vIn)
{
	float4 pos = float4(vIn.position, 0.0, 1.0);
	pos = mul(pos, transformation);
	pos = mul(pos, projection);

	VertexOut vOut;
	vOut.position = pos;

	return vOut;
}
