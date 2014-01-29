#include "ParticleSystemShared.hlsli"

cbuffer cbFixed : register (b0)
{
	// Net constant acceleration used to accelerate the particles.
	float3 gAccelW;
	float padding1;
};

struct Particle
{
	float3 InitialPosW	: POSITION;
	float3 InitialVelW	: VELOCITY;
	float2 SizeW		: SIZE;
	float Age			: AGE;
	uint Type			: TYPE;
};

struct VertexOut
{
	float3 PosW  : POSITION;
	float2 SizeW : SIZE;
	float4 Color : COLOR;
	uint   Type  : TYPE;
};

VertexOut main(Particle vIn)
{
	VertexOut vOut;
	float t = vIn.Age;

	// constant acceleration equation
	vOut.PosW = 0.5f*t*t*gAccelW + t*vIn.InitialVelW + vIn.InitialPosW;

	// fade color with time
	float opacity = 1.0f - smoothstep(0.0f, 1.0f, t / 1.0f);
	vOut.Color = float4(1.0f, 1.0f, 1.0f, opacity);

	vOut.SizeW = vIn.SizeW;
	vOut.Type = vIn.Type;

	return vOut;
}