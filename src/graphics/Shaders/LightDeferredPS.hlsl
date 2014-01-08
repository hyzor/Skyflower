#include "LightDeferredVS.hlsl"
#include "LightDef.hlsli"

cbuffer cLightBuffer : register(b0)
{
	PointLight gPointLights[MAX_POINT_LIGHTS];
	int gPointLightCount;
	int padding2, padding3, padding4;

	DirectionalLight gDirLights[MAX_DIR_LIGHTS];
	int gDirLightCount;
	int padding5, padding6, padding7;

	SpotLight gSpotLights[MAX_SPOT_LIGHTS];
	int gSpotLightCount;
	int padding8, padding9, padding10;

	float3 gEyePosW;
	float padding;
};

Texture2D gDiffuseTexture : register(t0);
Texture2D gNormalTexture : register(t1);
Texture2D gSpecularTexture : register(t2);
Texture2D gPositionTexture : register(t3);

SamplerState samLinear : register(s0);
SamplerState samAnisotropic : register(s1);

float4 main(VertexOut pIn) : SV_TARGET
{
	float4 diffuse;
	float3 normal;
	float4 specular;
	float3 position;
	
	diffuse = gDiffuseTexture.Sample(samLinear, pIn.Tex);
	normal = gNormalTexture.Sample(samLinear, pIn.Tex).xyz;
	specular = gSpecularTexture.Sample(samLinear, pIn.Tex);
	position = gPositionTexture.Sample(samLinear, pIn.Tex).xyz;

	// The toEye vector is used in lighting
	float3 toEye = gEyePosW - position;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye);

	// Normalize
	toEye /= distToEye;

	float4 litColor = diffuse;

	float4 ambient_Lights = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse_Lights = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular_Lights = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float4 A, D, S;

	// Begin calculating lights
	for (int i = 0; i < gDirLightCount; ++i)
	{
		ComputeDirectionalLight_Deferred(specular, gDirLights[i], normal, toEye, D, S);
		diffuse_Lights += D;
		specular_Lights += S;
	}

	for (int j = 0; j < gPointLightCount; ++j)
	{
		ComputePointLight_Deferred(specular, gPointLights[j], position, normal, toEye, D, S);
		diffuse_Lights += D;
		specular_Lights += S;
	}

	for (int k = 0; k < gSpotLightCount; ++k)
	{
		ComputeSpotLight_Deferred(specular, gSpotLights[k], position, normal, toEye, A, D, S);
		ambient_Lights += A;
		diffuse_Lights += D;
		specular_Lights += S;
	}

	litColor = diffuse * (ambient_Lights + diffuse_Lights) + specular_Lights;

	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
	litColor.a = diffuse.a;
	return litColor;
}