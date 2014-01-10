#include "ShadowBuildVS.hlsl"

float4 main(VertexOut pIn) : SV_TARGET
{
	//float4 greyScale(pIn.Pos.z, pIn.Pos.z, pIn.Pos.z, 1.0f);

	return float4(pIn.Pos.z, pIn.Pos.z, pIn.Pos.z, 1.0f);
}