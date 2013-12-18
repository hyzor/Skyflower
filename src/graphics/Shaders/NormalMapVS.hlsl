cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	float4x4 gTexTransform;
};

struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex : TEXCOORD;
	float4 TangentL : TANGENT;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 Tex : TEXCOORD;
	float4 TangentW : TANGENT;
};

VertexOut main(VertexIn vIn)
{
	VertexOut vOut;

	// Transform to world space space
	vOut.PosW = mul(float4(vIn.PosL, 1.0f), gWorld).xyz;
	vOut.NormalW = mul(vIn.NormalL, (float3x3)gWorldInvTranspose);
	vOut.TangentW = mul(vIn.TangentL, gWorld);

	// Transform to homogeneous clip space
	vOut.PosH = mul(float4(vIn.PosL, 1.0f), gWorldViewProj);

	// Output vertex attributes for interpolation across triangle
	vOut.Tex = mul(float4(vIn.Tex, 0.0f, 1.0f), gTexTransform).xy;

	// Generate projective tex coords to project shadow map onto scene
	//vOut.ShadowPosH = mul(float4(vIn.PosL, 1.0f), gShadowTransform);

	return vOut;
}