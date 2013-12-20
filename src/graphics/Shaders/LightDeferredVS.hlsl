cbuffer cbPerObject : register(b0)
{
	//float4x4 gWorld;
	//float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	//float4x4 gWorldViewProjTex;
	//float4x4 gTexTransform;
	//float4x4 gShadowTransform;
};

struct VertexIn
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD0;
};

struct VertexOut
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
	//float4 ShadowPosH : TEXCOORD1;
};

VertexOut main(VertexIn vIn)
{
	VertexOut vOut;

	vIn.Pos.w = 1.0f;
	// Transform to homogeneous clip space
	vOut.Pos = mul(vIn.Pos, gWorldViewProj);

	// Output vertex attributes for interpolation across triangle
	//vOut.Tex = mul(float4(vIn.Tex, 0.0f, 1.0f), gTexTransform).xy;

	// Store texture coordinates for pixel shader
	vOut.Tex = vIn.Tex;

	// Generate projective tex coords to project shadow map onto scene
	//vOut.ShadowPosH = mul(float4(vIn.PosL, 1.0f), gShadowTransform);

	return vOut;
}