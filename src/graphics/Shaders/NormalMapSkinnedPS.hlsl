#include "LightDef.hlsli"
#include "NormalMapSkinnedVS.hlsl"

cbuffer cbPerFrame : register(b0)
{
	PLight gPLights[MAX_POINT_LIGHTS];
	int gPLightCount;
	int padding2, padding3, padding4;

	DLight gDirLights[MAX_DIR_LIGHTS];
	int gDirLightCount;
	int padding5, padding6, padding7;

	SLight gSLights[MAX_SPOT_LIGHTS];
	int gSLightCount;
	int padding8, padding9, padding13;

	float3 gEyePosW;
	float padding;
};

cbuffer cbPerObject : register(b1)
{
	Material gMaterial;
};

Texture2D gDiffuseMap : register(t0);
Texture2D gNormalMap : register(t1);
Texture2D gShadowMap : register(t2);

SamplerState samLinear : register(s0);
SamplerComparisonState samShadow : register(s1);

float4 main(VertexOut pIn) : SV_TARGET
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
		//if (gUseTexture)
		//{
		// Sample texture
		texColor = gDiffuseMap.Sample(samLinear, pIn.Tex);

	//if (gAlphaClip)
	//{
	// Discard pixel if alpha < 0.1
	//clip(texColor.a - 0.1f);
	//}
	//}

	//--------------------------------------------------------
	// Normal mapping
	//--------------------------------------------------------
	float3 normalMapSample = gNormalMap.Sample(samLinear, pIn.Tex).rgb;
	float3 bumpedNormalW = NormalSampleToWorldSpace(normalMapSample, pIn.NormalW, pIn.TangentW);

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

			float3 shadow = float3(1.0f, 1.0f, 1.0f);
			shadow[0] = CalcShadowFactor(samShadow, gShadowMap, pIn.ShadowPosH);

			float4 A, D, S;
		for (int j = 0; j < gPLightCount; ++j)
		{
			ComputePLight(gMaterial, gPLights[j], pIn.PosW, pIn.NormalW, toEye, A, D, S);

			ambient += A;
			diffuse += D;
			specular += S;
		}

		for (int k = 0; k < gDirLightCount; ++k)
		{
			ComputeDLight(gMaterial, gDirLights[k], pIn.NormalW, toEye, A, D, S);

			ambient += A;
			diffuse += shadow[k] * D;
			specular += shadow[k] * S;
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