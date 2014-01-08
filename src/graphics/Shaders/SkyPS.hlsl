#include "SkyVS.hlsl"

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

float4 main(VertexOut pIn) : SV_TARGET
{
	float4 Color = gCubeMap.Sample(samTriLinearSam, pIn.PosL);

	return Color;
}