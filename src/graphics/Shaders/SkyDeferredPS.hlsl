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

struct PixelOut
{
	float4 Color : SV_Target0;
	float4 Normal : SV_Target1;
};

PixelOut main(VertexOut pIn) : SV_TARGET
{
	PixelOut pOut;

	pOut.Color = gCubeMap.Sample(samTriLinearSam, pIn.PosL);

	// Skybox "always in shadow"
	pOut.Color.w = 0.0f;

	pOut.Normal.xyz = -1.0f;

	// Diffuse is initially fully lit
	pOut.Normal.w = 1.0f;

	return pOut;
}