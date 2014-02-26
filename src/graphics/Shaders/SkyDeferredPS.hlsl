#include "SkyDeferredVS.hlsl"

// Cube map
TextureCube gCubeMap;

SamplerState samTriLinearSam : register(s0);

/*
SamplerState samTriLinearSam
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};
*/

struct PixelOut
{
	float4 Color : SV_Target0;
	float4 Normal : SV_Target1;
	//float4 Specular : SV_Target2;
	float2 Velocity : SV_Target2;
};

PixelOut main(VertexOut pIn)// : SV_TARGET
{
	PixelOut pOut;

	pOut.Color = gCubeMap.Sample(samTriLinearSam, pIn.PosL);

	// Default material
	pOut.Color.w = 1.0f;

	// No shadow
	pOut.Normal.xyz = 1.0f;

	pOut.Normal.w = 1.0f;

	//pOut.Specular = float4(0.0f, 0.0f, 0.0f, 1.0f);

	// Gamma correct color (make it linear)
	pOut.Color.xyz = pow(pOut.Color.xyz, 2.2f);

	float2 CurPosXY;
	float2 PrevPosXY;

	CurPosXY = (pIn.CurPosH.xy / pIn.CurPosH.w) * 0.5f + 0.5f;
	PrevPosXY = (pIn.PrevPosH.xy / pIn.PrevPosH.w) * 0.5f + 0.5f;

	pOut.Velocity = (CurPosXY - PrevPosXY) * 0.5f + 0.5f;
	pOut.Velocity = pow(pOut.Velocity, 3.0f);

	return pOut;
}