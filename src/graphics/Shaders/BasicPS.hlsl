#include "LightDef.hlsli"
#include "BasicVS.hlsl"


cbuffer cbPerObject : register(b0)
{
	Material gMaterial;
};

cbuffer cbPerFrame : register(b1)
{
	PLight gPLights[MAX_POINT_LIGHTS];
	int gPLightCount;
	int padding2, padding3, padding4;

	DLight gDirLights[MAX_DIR_LIGHTS];
	int gDirLightCount;
	int padding5, padding6, padding7;

	SLight gSLights[MAX_SPOT_LIGHTS];
	int gSLightCount;
	int padding8, padding9, padding10;

	float3 gEyePosW;
	float padding;
};

//Texture2D gShadowMap;
//TextureCube gCubeMap;

Texture2D gDiffuseMap : register(t0);
Texture2D gShadowMap : register(t1);

SamplerState samLinear : register(s0);
SamplerState samAnisotropic : register(s1);
SamplerComparisonState samShadow : register(s2);

/*
SamplerState samLinear
{
Filter = MIN_MAG_MIP_LINEAR;
AddressU = WRAP;
AddressV = WRAP;
};

SamplerState samAnisotropic
{
Filter = ANISOTROPIC;
MaxAnisotropy = 4;

AddressU = WRAP;
AddressV = WRAP;
};
*/

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
	texColor = gDiffuseMap.Sample(samAnisotropic, pIn.Tex);

	//if (gAlphaClip)
	//{
	// Discard pixel if alpha < 0.1
	//clip(texColor.a - 0.1f);
	//}
	//}

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
			diffuse += /*shadow[j]**/D;
			specular += /*shadow[j]**/S;
		}
		[unroll]
		for (int k = 0; k < 1; ++k)
		{
			ComputeDLight(gMaterial, gDirLights[k], pIn.NormalW, toEye, A, D, S);

			ambient += A;
			diffuse += shadow[k]*D;
			specular += shadow[k]*S;
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

/*
float4 main() : SV_TARGET
{
return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
*/