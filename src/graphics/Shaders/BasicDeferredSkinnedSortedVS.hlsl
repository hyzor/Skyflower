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

/*
cbuffer cbSkinned : register(b1)
{
	float4x4 gBoneTransforms[96];
	int numBoneTransforms;
	int padding10, padding11, padding12;
};
*/

cbuffer cbSkinned : register(b1)
{
	float4x4 gUpperBodyBoneTransforms[64];
	unsigned int numUpperBodyBoneTransforms;

	unsigned int rootBoneIndex;
	float2 padding;

	float4x4 gLowerBodyBoneTransforms[64];
	unsigned int numLowerBodyBoneTransforms;
	float3 paddingLower;
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
	float4 CurPosH : CURPOSH;
	float4 PrevPosH : PREVPOSH;
	float Depth : TEXCOORD4;
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

	// Corresponding lower bone transformations in upper body bone transformations matrix is the identity matrix,
	// so it wont affect the transformation. Vice versa.

	for (int i = 0; i < 4; ++i)
	{
		float4 modifiedPosL_Lower = float4(0.0f, 0.0f, 0.0f, 1.0f);
		modifiedPosL_Lower.xyz = mul(float4(vIn.PosL, 1.0f), gLowerBodyBoneTransforms[vIn.BoneIndices[i]]).xyz;

		float4 modifiedPosL_Upper = float4(0.0f, 0.0f, 0.0f, 1.0f);
		modifiedPosL_Upper.xyz = mul(float4(vIn.PosL, 1.0f), gUpperBodyBoneTransforms[vIn.BoneIndices[i]]).xyz;

		bool modified = false;

		// Check if not root bone
		if (vIn.BoneIndices[i] != rootBoneIndex)
		{
			// Check if the modified local position is different from the local position (bind pose)
			if (modifiedPosL_Lower.x != vIn.PosL.x && modifiedPosL_Lower.y != vIn.PosL.y && modifiedPosL_Lower.z != vIn.PosL.z)
			{
				// Transforming with lower body transformations
				posL += weights[i] * modifiedPosL_Lower.xyz;
				normalL += weights[i] * mul(vIn.NormalL, (float3x3)gLowerBodyBoneTransforms[vIn.BoneIndices[i]]);
				tangentL += weights[i] * mul(vIn.TangentL.xyz, (float3x3)gLowerBodyBoneTransforms[vIn.BoneIndices[i]]);

				modified = true;
			}

			// Check if the modified local position is different from the local position (bind pose)
			if (modifiedPosL_Upper.x != vIn.PosL.x && modifiedPosL_Upper.y != vIn.PosL.y && modifiedPosL_Upper.z != vIn.PosL.z)
			{
				// Now transform with upper body transformations
				posL += weights[i] * modifiedPosL_Upper.xyz;
				normalL += weights[i] * mul(vIn.NormalL, (float3x3)gUpperBodyBoneTransforms[vIn.BoneIndices[i]]);
				tangentL += weights[i] * mul(vIn.TangentL.xyz, (float3x3)gUpperBodyBoneTransforms[vIn.BoneIndices[i]]);

				modified = true;
			}
		}

		// Root bone, interpolate between lower and upper body transformations
		else
		{
			float3 posL_Lower = float3(0.0f, 0.0f, 0.0f);
			float3 posL_Upper = float3(0.0f, 0.0f, 0.0f);

			float3 normalL_Lower = float3(0.0f, 0.0f, 0.0f);
			float3 normalL_Upper = float3(0.0f, 0.0f, 0.0f);

			float3 tangentL_Lower = float3(0.0f, 0.0f, 0.0f);
			float3 tangentL_Upper = float3(0.0f, 0.0f, 0.0f);

			// Start by transforming with lower body transformations
			posL_Lower += weights[i] * modifiedPosL_Lower.xyz;
			normalL_Lower += weights[i] * mul(vIn.NormalL, (float3x3)gLowerBodyBoneTransforms[vIn.BoneIndices[i]]);
			tangentL_Lower += weights[i] * mul(vIn.TangentL.xyz, (float3x3)gLowerBodyBoneTransforms[vIn.BoneIndices[i]]);

			// Now transform with upper body transformations
			posL_Upper += weights[i] * modifiedPosL_Upper.xyz;
			normalL_Upper += weights[i] * mul(vIn.NormalL, (float3x3)gUpperBodyBoneTransforms[vIn.BoneIndices[i]]);
			tangentL_Upper += weights[i] * mul(vIn.TangentL.xyz, (float3x3)gUpperBodyBoneTransforms[vIn.BoneIndices[i]]);

			posL += lerp(posL_Lower, posL_Upper, float3(0.5f, 0.5f, 0.5f));
			normalL += lerp(normalL_Lower, normalL_Upper, float3(0.5f, 0.5f, 0.5f));
			tangentL += lerp(tangentL_Lower, tangentL_Upper, float3(0.5f, 0.5f, 0.5f));
		}
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

	vOut.CurPosH = vOut.PosH;
	//vOut.CurPosH.xy /= vOut.CurPosH.w;

	vOut.PrevPosH = mul(float4(posL, 1.0f), gPrevWorldViewProj);
	//vOut.PrevPosH.xy /= vOut.PrevPosH.w;

	return vOut;
}