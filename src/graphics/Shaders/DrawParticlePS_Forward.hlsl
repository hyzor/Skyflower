#include "DrawParticleGS.hlsl"

float4 main(GeoOut pIn) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}

/*

Texture2DArray gTexArray : register(t0);

SamplerState samLinear : register(s0);

cbuffer cbPerFrame : register(b0)
{
	PLight gPLights[MAX_POINT_LIGHTS];
	int gPLightCount;
	float3 padding1;

	DLight gDirLights[MAX_DIR_LIGHTS];
	int gDirLightCount;
	float3 padding2;

	SLight gSLights[MAX_SPOT_LIGHTS];
	int gSLightCount;
	float3 padding3;

	float3 gEyePosW;
	float padding4;
};

float4 main(GeoOut pIn) : SV_TARGET
{
	// Normalize normal
	pIn.NormalW = normalize(pIn.NormalW);

	// The toEye vector is used in lighting
	float3 toEye = gEyePosW - pIn.PosW;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye);

	// Normalize
	toEye /= distToEye;

	float4 texColor = float4(1, 1, 1, 1);

	texColor = gTexArray.Sample(samLinear, float3(pIn.Tex, pIn.TexIndex)) * pIn.Color;

	//--------------------------------------------------------
	// Lighting
	//--------------------------------------------------------

	float4 litColor = texColor;
	if (gPLightCount > 0 || gDirLightCount > 0)
	{
		// Initialize values
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

		//float3 shadow = float3(1.0f, 1.0f, 1.0f);
		//shadow[0] = CalcShadowFactor(samShadow, gShadowMap, pIn.ShadowPosH);

		float4 A, D, S;
		for (int j = 0; j < gPLightCount; ++j)
		{
			ComputePLight(gMaterial, gPLights[j], pIn.PosW, pIn.NormalW, toEye, A, D, S);

			ambient += A;
			diffuse += D;
			specular += S;
		}
		[unroll]
		for (int k = 0; k < 1; ++k)
		{
			ComputeDLight(gMaterial, gDirLights[k], pIn.NormalW, toEye, A, D, S);

			ambient += A;
			diffuse += * D;
			specular += * S;
		}

		for (int i = 0; i < gSLightCount; ++i)
		{
			ComputeSLight(gMaterial, gSLights[i], pIn.PosW, pIn.NormalW, toEye, A, D, S);
			ambient += A;
			diffuse += D;
			specular += S;
		}

		litColor = texColor * (ambient + diffuse) + specular;
	}

	litColor.a = gMaterial.Diffuse.a * texColor.a;

	return litColor;
}
}
*/