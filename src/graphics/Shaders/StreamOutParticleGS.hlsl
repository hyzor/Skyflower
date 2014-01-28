#include "ParticleSystemShared.hlsli"
#include "StreamOutParticleVS.hlsl"

cbuffer cbPerFrame
{
	float3 gEyePosW;
	float gGameTime;

	// for when the emit position/direction is varying
	float3 gEmitPosW;
	float gTimeStep;

	float3 gEmitDirW;
	float padding;

};

// Random texture used to generate random numbers in shaders.
Texture1D gRandomTex : register(t0);

SamplerState samLinear : register(s0);

float3 RandUnitVec3(float offset)
{
	// Use game time plus offset to sample random texture.
	float u = (gGameTime + offset);

	// coordinates in [-1,1]
	float3 v = gRandomTex.SampleLevel(samLinear, u, 0).xyz;

	// project onto unit sphere
	return normalize(v);
}

[maxvertexcount(2)]
void main(point Particle gIn[1], inout PointStream<Particle> ptStream)
{
	gIn[0].Age += gTimeStep;

	if (gIn[0].Type == PT_EMITTER)
	{
		// Check if time to emit a new particle
		if (gIn[0].Age > 0.005f)
		{
			float3 vRandom = RandUnitVec3(0.0f);
			vRandom.x *= 0.5f;
			vRandom.z *= 0.5f;

			Particle p;
			p.InitialPosW = gEmitPosW.xyz;
			p.InitialVelW = 4.0f*vRandom;
			p.SizeW = float2(3.0f, 3.0f);
			p.Age = 0.0f;
			p.Type = PT_FLARE;

			ptStream.Append(p);

			// Reset the time to emit
			gIn[0].Age = 0.0f;
		}

		// Always keep emitters
		ptStream.Append(gIn[0]);
	}
	else
	{
		// Specify conditions to keep particle; this may vary from system to system.
		if (gIn[0].Age <= 1.0f)
			ptStream.Append(gIn[0]);
	}
}