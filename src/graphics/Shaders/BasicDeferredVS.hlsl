#include "CascadedShadowsShared.hlsli"

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	//float4x4 gWorldViewProjTex;
	float4x4 gTexTransform;
	float4x4 gShadowTransforms[MAX_CASCADES]; //Transform for each cascade's individual transform
	float4x4 gPrevWorldViewProj;
	float4x4 gToEyeSpace;
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
	float4 ShadowPosH1 : TEXCOORD1;
	float4 ShadowPosH2 : TEXCOORD2;
	float4 ShadowPosH3 : TEXCOORD3;
	float4 CurPosH : CURPOSH;
	float4 PrevPosH : PREVPOSH;
	float Depth : TEXCOORD4;
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

	// Store texture coordinates for pixel shader
	vOut.Tex = vIn.Tex;

	vOut.CurPosH = vOut.PosH;
	vOut.PrevPosH = mul(float4(vIn.PosL, 1.0f), gPrevWorldViewProj);

	//Generate depth for current pixel in cam space
	vOut.Depth = mul(float4(vOut.PosW, 1.0f), gToEyeSpace).z;

	//Generate coordinates to use when sampling shadowmap, for each different shadowmap
	vOut.ShadowPosH1 = mul(float4(vOut.PosW, 1.0f), gShadowTransforms[0]);
	vOut.ShadowPosH2 = mul(float4(vOut.PosW, 1.0f), gShadowTransforms[1]);
	vOut.ShadowPosH3 = mul(float4(vOut.PosW, 1.0f), gShadowTransforms[2]);

	return vOut;
}