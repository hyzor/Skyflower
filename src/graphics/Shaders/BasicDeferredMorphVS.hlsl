// http://shiba.hpe.cn/jiaoyanzu/wuli/soft/Hlsl/Character-Animation-With-Direct3D.pdf

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	float4x4 gTexTransform;
	float4x4 gShadowTransform;

	// Morph target weights
	float4 gWeights;
};

struct VertexIn
{
	float3 basePosL : POSITION;
	float3 baseNormalL : NORMAL;
	float2 baseTex : TEXCOORD;

	float3 target1PosL : POSITION1;
	float3 target1NormalL : NORMAL1;
	float2 target1Tex : TEXCOORD1;

	float3 target2PosL : POSITION2;
	float3 target2NormalL : NORMAL2;
	float2 target2Tex : TEXCOORD2;

	float3 target3PosL : POSITION3;
	float3 target3NormalL : NORMAL3;
	float2 target3Tex : TEXCOORD3;

	float3 target4PosL : POSITION4;
	float3 target4NormalL : NORMAL4;
	float2 target4Tex : TEXCOORD4;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 Tex : TEXCOORD;
	float4 ShadowPosH : TEXCOORD1;
};

VertexOut main(VertexIn vIn)
{
	VertexOut vOut;

	// Calculate position, normal and tangent space
	float3 posL = float3(0.0f, 0.0f, 0.0f);
	float3 normalL = float3(0.0f, 0.0f, 0.0f);
	float3 tangentL = float3(0.0f, 0.0f, 0.0f);

	// First target
	posL += gWeights[0] * (vIn.target1PosL - vIn.basePosL.xyz);
	normalL += gWeights[0] * (vIn.target1NormalL - vIn.baseNormalL.xyz);
	//tangentL += gWeights[0] * (vIn.target1Tex - vIn.baseTex);

	// Second target
	posL += gWeights[1] * (vIn.target2PosL - vIn.basePosL.xyz);
	normalL += gWeights[1] * (vIn.target2NormalL - vIn.baseNormalL.xyz);
	//tangentL += gWeights[1] * (vIn.target2Tex - vIn.baseTex);

	// Third target
	posL += gWeights[2] * (vIn.target3PosL - vIn.basePosL.xyz);
	normalL += gWeights[2] * (vIn.target3NormalL - vIn.baseNormalL.xyz);
	//tangentL += gWeights[2] * (vIn.target3Tex - vIn.baseTex);

	// Fourth target
	posL += gWeights[3] * (vIn.target4PosL - vIn.basePosL.xyz);
	normalL += gWeights[3] * (vIn.target4NormalL - vIn.baseNormalL.xyz);
	//tangentL += gWeights[3] * (vIn.target4Tex - vIn.baseTex);

	// Now transform them to world space space
	vOut.PosW = mul(float4(posL, 1.0f), gWorld).xyz;
	vOut.NormalW = mul(normalL, (float3x3)gWorldInvTranspose);
	//vOut.TangentW = float4(mul(tangentL, (float3x3)gWorld), vIn.TangentL.w);

	// Normalize normal
	vOut.NormalW = normalize(vOut.NormalW);

	// Transform to homogeneous clip space
	vOut.PosH = mul(float4(posL, 1.0f), gWorldViewProj);

	// Output vertex attributes for interpolation across triangle
	//vOut.Tex = mul(float4(vIn.Tex, 0.0f, 1.0f), gTexTransform).xy;
	vOut.Tex = vIn.baseTex;

	// Generate projective tex coords to project shadow map onto scene
	vOut.ShadowPosH = mul(float4(posL, 1.0f), gShadowTransform);

	return vOut;
}