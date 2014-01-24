// http://shiba.hpe.cn/jiaoyanzu/wuli/soft/Hlsl/Character-Animation-With-Direct3D.pdf

cbuffer cbPerObject : register(b0)
{
	float4x4 gLightWorldViewProj;

	// Morph target weights
	float4 gWeights;
};

struct VertexIn
{
	float3 basePosL : POSITION0;
	float3 baseNormalL : NORMAL0;
	float2 baseTex : TEXCOORD0;

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
	float4 Pos : SV_POSITION;
};

VertexOut main(VertexIn vIn)
{
	VertexOut vOut;

	// Calculate position, normal and tangent space
	float3 posL = float3(0.0f, 0.0f, 0.0f);

	posL = vIn.basePosL;

	// First target
	posL += gWeights.x * (vIn.target1PosL - vIn.basePosL);

	// Second target
	posL += gWeights.y * (vIn.target2PosL - vIn.basePosL);

	// Third target
	posL += gWeights.z * (vIn.target3PosL - vIn.basePosL);

	// Fourth target
	posL += gWeights.w * (vIn.target4PosL - vIn.basePosL);

	// Transform to homogeneous clip space
	vOut.Pos = mul(float4(posL, 1.0f), gLightWorldViewProj);

	return vOut;
}