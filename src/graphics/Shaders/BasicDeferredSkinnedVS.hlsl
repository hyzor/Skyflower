cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	float4x4 gTexTransform;
	float4x4 gShadowTransform;

	float4x4 gPrevWorldViewProj;
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
	float4 ShadowPosH : TEXCOORD1;

	float4 CurPosH : CURPOSH;
	float4 PrevPosH : PREVPOSH;
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

	// Generate projective tex coords to project shadow map onto scene
	vOut.ShadowPosH = mul(float4(posL, 1.0f), gShadowTransform);

	vOut.CurPosH = vOut.PosH;
	//vOut.CurPosH.xy /= vOut.CurPosH.w;

	vOut.PrevPosH = mul(float4(vIn.PosL, 1.0f), gPrevWorldViewProj);
	//vOut.PrevPosH.xy /= vOut.PrevPosH.w;

	return vOut;
}