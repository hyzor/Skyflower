#include "ParticleSystemShared.hlsli"
#include "DrawParticleVS.hlsl"

cbuffer cbPerFrame : register(b0)
{
	float3 gEyePosW;
	float padding;

	float4x4 gViewProj;

	float4 gQuadTexC[4];

	unsigned int gTextureIndex;
	float3 paddingTex;
};

struct GeoOut
{
	float4 PosH  : SV_Position;
	float4 Color : COLOR;
	float2 Tex   : TEXCOORD;
	unsigned int TexIndex : TEXINDEX;
};

// The draw GS just expands points into camera facing quads.
[maxvertexcount(4)]
void main(point VertexOut gin[1],
	inout TriangleStream<GeoOut> triStream)
{
	// Do not draw emitter particles.
	if (gin[0].Type != PT_EMITTER)
	{
		//
		// Compute world matrix so that billboard faces the camera.
		//
		float3 look = normalize(gEyePosW.xyz - gin[0].PosW);
		float3 right = normalize(cross(float3(0, 1, 0), look));
		float3 up = cross(look, right);

		//
		// Compute triangle strip vertices (quad) in world space.
		//
		float halfWidth = 0.5f*gin[0].SizeW.x;
		float halfHeight = 0.5f*gin[0].SizeW.y;

		float4 v[4];
		v[0] = float4(gin[0].PosW + halfWidth*right - halfHeight*up, 1.0f);
		v[1] = float4(gin[0].PosW + halfWidth*right + halfHeight*up, 1.0f);
		v[2] = float4(gin[0].PosW - halfWidth*right - halfHeight*up, 1.0f);
		v[3] = float4(gin[0].PosW - halfWidth*right + halfHeight*up, 1.0f);

		//
		// Transform quad vertices to world space and output 
		// them as a triangle strip.
		//
		GeoOut gout;
		[unroll]
		for (int i = 0; i < 4; ++i)
		{
			gout.PosH = mul(v[i], gViewProj);
			gout.Tex = gQuadTexC[i].xy;
			gout.Color = gin[0].Color;
			gout.TexIndex = gin[0].Type - 1;
			triStream.Append(gout);
		}
	}
}