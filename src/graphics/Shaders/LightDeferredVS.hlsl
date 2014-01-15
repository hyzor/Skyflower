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

	// Generate projective tex coords to project shadow map onto scene
	//vOut.ShadowPosH = mul(vIn.Pos, gShadowTransform);

	float4 screenPos;
	screenPos.x = vIn.Tex.x * 2.0f - 1.0f;
	screenPos.y = -(vIn.Tex.y * 2.0f - 1.0f);
	screenPos.z = 1.0f;
	screenPos.w = 1.0f;

	// Transform to homogeneous clip space
	vOut.Pos = mul(vIn.Pos, gWorldViewProj);

	vOut.ProjTex = mul(vIn.Pos, gLightViewProj);

	float4 worldPos = mul(vOut.Pos, gViewProjInv);
		worldPos /= worldPos.w;

	vOut.shadowPosH = mul(worldPos, gLightViewProj);

	// Output vertex attributes for interpolation across triangle
	//vOut.Tex = mul(float4(vIn.Tex, 0.0f, 1.0f), gTexTransform).xy;

	// Store texture coordinates for pixel shader
	vOut.Tex = vIn.Tex;



	return vOut;
}