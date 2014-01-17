cbuffer cbPerObject : register(b0)
{
	//float4x4 gWorld;
	//float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	//float4x4 gWorldViewProjTex;
	//float4x4 gTexTransform;
	float4x4 gShadowTransform;
	float4x4 gLightViewProj;
	float4x4 gViewProjInv;
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
	float4 ProjTex : POSITION;
	float4 shadowPosH : POSITION2;
};

VertexOut main(VertexIn vIn)
{
	VertexOut vOut;

	vIn.Pos.w = 1.0f;

	// Transform to homogeneous clip space
	vOut.Pos = mul(vIn.Pos, gWorldViewProj);

	// Store texture coordinates for pixel shader
	vOut.Tex = vIn.Tex;



	return vOut;
}