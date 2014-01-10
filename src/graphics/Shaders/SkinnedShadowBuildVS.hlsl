cbuffer cbPerObject : register(b0)
{
	float4x4 gLightWorldViewProj;
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
	//float3 NormalL : NORMAL;
	//float2 Tex : TEXCOORD;
	//float4 TangentL : TANGENT;
	float3 Weights : WEIGHTS;
	uint4 BoneIndices : BONEINDICES;
};

struct VertexOut
{
	float4 Pos : SV_POSITION;
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
		//float3 normalL = float3(0.0f, 0.0f, 0.0f);
		//float3 tangentL = float3(0.0f, 0.0f, 0.0f);


	for (int i = 0; i < 4; ++i)
	{
		posL += weights[i] * mul(float4(vIn.PosL, 1.0f), gBoneTransforms[vIn.BoneIndices[i]]).xyz;
		//normalL += weights[i] * mul(vIn.NormalL, (float3x3)gBoneTransforms[vIn.BoneIndices[i]]);
		//tangentL += weights[i] * mul(vIn.TangentL.xyz, (float3x3)gBoneTransforms[vIn.BoneIndices[i]]);
	}


	// Now transform them to world space space
	//vOut.PosW = mul(float4(posL, 1.0f), gWorld).xyz;
	vOut.Pos = mul(float4(posL, 1.0f), gLightWorldViewProj);
	//vOut.NormalW = mul(normalL, (float3x3)gWorldInvTranspose);
	//vOut.TangentW = float4(mul(tangentL, (float3x3)gWorld), vIn.TangentL.w);

	return vOut;
}