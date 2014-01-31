cbuffer cbPerFrame
{
	float4x4 gWorldViewProj;
	float4x4 gPrevWorldViewProj;
};

struct VertexIn
{
	float3 PosL : POSITION;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosL : POSITION;

	float4 CurPosH : CURPOSH;
	float4 PrevPosH : PREVPOSH;
};

VertexOut main(VertexIn vIn)
{
	VertexOut vout;

	// Set z = w so that z/w = 1 (i.e., skydome always on far plane).
	vout.PosH = mul(float4(vIn.PosL, 1.0f), gWorldViewProj).xyww;

	// Use local vertex position as cubemap lookup vector.
	vout.PosL = vIn.PosL;

	vout.CurPosH = vout.PosH;
	vout.PrevPosH = mul(float4(vIn.PosL, 1.0f), gPrevWorldViewProj).xyww;

	return vout;
}