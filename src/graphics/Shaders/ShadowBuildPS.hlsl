#include "ShadowBuildVS.hlsl"

float4 main(VertexOut pIn) : SV_TARGET
{
	return pIn.Pos;
}