#include "CascadedShadowsShared.hlsli"

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	//float4x4 gWorldViewProjTex;
	float4x4 gTexTransform;
	float4x4 gShadowTransform;
	float4x4 gShadowTransforms[MAX_CASCADES]; //Transform for each cascade's individual projection
	float4x4 gPrevWorldViewProj;
	
	float4x4 gWorldView;
	float4x4 gShadowSpace; //Regardless of cascade, transform to lightspace
};

struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 Tex : TEXCOORD0;
	float4 ShadowPosH : TEXCOORD1;
	float4 ShadowPosH1 : TEXCOORD2;
	float4 ShadowPosH2 : TEXCOORD3;
	float4 ShadowPosH3 : TEXCOORD4;

	float4 CurPosH : CURPOSH;
	float4 PrevPosH : PREVPOSH;

	float Depth : TEXCOORD5;

	//float2 CurPosXY : CURPOSXY;
	//float2 PrevPosXY : PREVPOSXY;

	//float2 Velocity : VELOCITY;
};

VertexOut main(VertexIn vIn)
{
	VertexOut vOut;

	// Transform to world space space
	vOut.PosW = mul(float4(vIn.PosL, 1.0f), gWorld).xyz;
	vOut.NormalW = mul(vIn.NormalL, (float3x3)gWorldInvTranspose);
	
	// Normalize normal
	vOut.NormalW = normalize(vOut.NormalW);

	// Transform to homogeneous clip space
	vOut.PosH = mul(float4(vIn.PosL, 1.0f), gWorldViewProj);

	// Output vertex attributes for interpolation across triangle
	//vOut.Tex = mul(float4(vIn.Tex, 0.0f, 1.0f), gTexTransform).xy;

	// Store texture coordinates for pixel shader
	vOut.Tex = vIn.Tex;

	// Generate projective tex coords to project shadow map onto scene
	vOut.ShadowPosH = mul(float4(vIn.PosL, 1.0f), gShadowTransform);

	//vOut.CurPosV = mul(float4(vOut.PosW, 1.0f), gViewProj);
	//vOut.PrevPosV = mul(float4(vOut.PosW, 1.0f), gPrevViewProj);

	//float4 CurPosH;
	//float4 PrevPosH;

	vOut.CurPosH = vOut.PosH;
	//vOut.CurPosH = (vOut.CurPosH.xy / vOut.CurPosH.w) * 0.5f + 0.5f;

	vOut.PrevPosH = mul(float4(vIn.PosL, 1.0f), gPrevWorldViewProj);
	//vOut.PrevPosH.xy = (vOut.PrevPosH.xy / vOut.PrevPosH.w) * 0.5f + 0.5f;
	//vOut.PrevPosH.xy /= vOut.PrevPosH.w;

	//Generate depth for current pixel in cam space
	vOut.Depth = mul(float4(vIn.PosL, 1.0), gWorldView).z;

	//Transform position to light/shadow -space
	//vOut.TexShadow = mul(float4(vIn.PosL, 1.0), gShadowSpace);

	vOut.ShadowPosH1 = mul(float4(vIn.PosL, 1.0f), gShadowTransforms[0]);
	vOut.ShadowPosH2 = mul(float4(vIn.PosL, 1.0f), gShadowTransforms[1]);
	vOut.ShadowPosH3 = mul(float4(vIn.PosL, 1.0f), gShadowTransforms[2]);

	//vOut.Velocity = vOut.CurPosH - vOut.PrevPosH;
	//vOut.Velocity /= 2.0f;

	//CurPosXY = (CurPosH.xy / CurPosH.w) * 0.5f + 0.5f;
	//PrevPosXY = (PrevPosH.xy / PrevPosH.w) * 0.5f + 0.5f;

	return vOut;
}