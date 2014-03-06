cbuffer cbPerObject : register(b0)
{
	float4x4 gLightWorldViewProj;
};

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

	// Calculate position
	float3 posL = float3(0.0f, 0.0f, 0.0f);

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

				modified = true;
			}

			// Check if the modified local position is different from the local position (bind pose)
			if (modifiedPosL_Upper.x != vIn.PosL.x && modifiedPosL_Upper.y != vIn.PosL.y && modifiedPosL_Upper.z != vIn.PosL.z)
			{
				// Now transform with upper body transformations
				posL += weights[i] * modifiedPosL_Upper.xyz;

				modified = true;
			}
		}

		// Root bone, interpolate between lower and upper body transformations
		else
		{
			float3 posL_Lower = float3(0.0f, 0.0f, 0.0f);
			float3 posL_Upper = float3(0.0f, 0.0f, 0.0f);

			// Start by transforming with lower body transformations
			posL_Lower += weights[i] * modifiedPosL_Lower.xyz;

			// Now transform with upper body transformations
			posL_Upper += weights[i] * modifiedPosL_Upper.xyz;

			posL += lerp(posL_Lower, posL_Upper, float3(0.5f, 0.5f, 0.5f));
		}
	}

	vOut.Pos = mul(float4(posL, 1.0f), gLightWorldViewProj);

	return vOut;
}