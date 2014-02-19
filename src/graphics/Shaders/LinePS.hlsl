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

float4 main(VertexOut pIn) : SV_TARGET
{
	return color;
}
