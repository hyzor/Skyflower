#include "CascadedShadowsShared.hlsli"

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	float4x4 gTexTransform;
	float4x4 gPrevWorldViewProj;
	float4x4 gShadowTransforms[MAX_CASCADES];
	float4x4 gToEyeSpace;
};

cbuffer cbSkinned : register(b1)
{
	float4x4 gBoneTransforms[96];
	int numBoneTransforms;
	int padding10, padding11, padding12;
};

struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex : TEXCOORD;
	float4 TangentL : TANGENT;
	float3 Weights : WEIGHTS;
	uint4 BoneIndices : BONEINDICES;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 Tex : TEXCOORD;
	float4 TangentW : TANGENT;
	float4 ShadowPosH1 : TEXCOORD1;
	float4 ShadowPosH2 : TEXCOORD2;
	float4 ShadowPosH3 : TEXCOORD3;
	float4 ShadowPosH4 : TEXCOORD4;
	float4 CurPosH : CURPOSH;
	float4 PrevPosH : PREVPOSH;
	float Depth : TEXCOORD5;
};

VertexOut main(VertexIn vIn)
{
	VertexOut vOut;

	// Init weights
	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights[0] = vIn.Weights.x;
	weights[1] = vIn.Weights.y;
	weights[2] = vIn.Weights.z;
	weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

	// Calculate position, normal and tangent space
	float3 posL = float3(0.0f, 0.0f, 0.0f);
	float3 normalL = float3(0.0f, 0.0f, 0.0f);
	float3 tangentL = float3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 4; ++i)
	{
		posL += weights[i] * mul(float4(vIn.PosL, 1.0f), gBoneTransforms[vIn.BoneIndices[i]]).xyz;
		normalL += weights[i] * mul(vIn.NormalL, (float3x3)gBoneTransforms[vIn.BoneIndices[i]]);
		tangentL += weights[i] * mul(vIn.TangentL.xyz, (float3x3)gBoneTransforms[vIn.BoneIndices[i]]);
	}

	// Now transform them to world space space
	vOut.PosW = mul(float4(posL, 1.0f), gWorld).xyz;
	vOut.NormalW = mul(normalL, (float3x3)gWorldInvTranspose);
	vOut.TangentW = float4(mul(tangentL, (float3x3)gWorld), vIn.TangentL.w);

	// Normalize normal
	vOut.NormalW = normalize(vOut.NormalW);

	// Transform to homogeneous clip space
	vOut.PosH = mul(float4(posL, 1.0f), gWorldViewProj);

	// Output vertex attributes for interpolation across triangle
	//vOut.Tex = mul(float4(vIn.Tex, 0.0f, 1.0f), gTexTransform).xy;
	vOut.Tex = vIn.Tex;

	//Generate depth for current pixel in cam space
	vOut.Depth = mul(float4(vOut.PosW, 1.0f), gToEyeSpace).z;

	// Generate projective tex coords to project shadow map onto scene
	vOut.ShadowPosH1 = mul(float4(vOut.PosW, 1.0f), gShadowTransforms[0]);
	vOut.ShadowPosH2 = mul(float4(vOut.PosW, 1.0f), gShadowTransforms[1]);
	vOut.ShadowPosH3 = mul(float4(vOut.PosW, 1.0f), gShadowTransforms[2]);
	vOut.ShadowPosH4 = mul(float4(vOut.PosW, 1.0f), gShadowTransforms[3]);

	vOut.CurPosH = vOut.PosH;
	//vOut.CurPosH.xy /= vOut.CurPosH.w;

	vOut.PrevPosH = mul(float4(posL, 1.0f), gPrevWorldViewProj);
	//vOut.PrevPosH.xy /= vOut.PrevPosH.w;

	return vOut;
}