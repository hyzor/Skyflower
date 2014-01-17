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

	float4x4 gShadowTransform_PS; // Light: view * projection * toTexSpace
	float4x4 gCameraView;
	float4x4 gCameraInvView;
	float4x4 gCameraWorld;
	float4x4 gCameraProj;
	float4x4 gLightWorld;
	float4x4 gLightView;
	float4x4 gLightInvView;
	float4x4 gLightProj;
};

Texture2D gDiffuseTexture : register(t0);
Texture2D gNormalTexture : register(t1);
Texture2D gSpecularTexture : register(t2);
Texture2D gPositionTexture : register(t3);
Texture2D gShadowMap : register(t5);
Texture2D gSSAOTexture : register(t4);

SamplerState samLinear : register(s0);
SamplerState samAnisotropic : register(s1);
SamplerComparisonState samShadow : register(s2);

// UNUSED
float ReadShadowMap(float3 eyeDir)
{
	float4x4 lightWorldViewProj = gLightProj * gLightView * gCameraInvView;
		//float4x4 final = gCameraInvView * lightWorldViewProj;
	//float4x4 final = (gLightProj)* (gLightInvView)* gCameraInvView;

		//float4 projectedEyeDir = mul(float4(eyeDir, 1.0f), final);

		float4 projectedEyeDir = mul(lightWorldViewProj, float4(eyeDir, 1.0f));

		//return CalcShadowFactor(samShadow, gShadowMap, projectedEyeDir);

	
		/*
	projectedEyeDir = projectedEyeDir / projectedEyeDir.w;

	float2 tex = projectedEyeDir.xy * float2(0.5f, 0.5f) + float2(0.5f, 0.5f);

	const float bias = 0.0001f;
	float depthValue = gShadowMap.Sample(samLinear, tex).x - bias;
	return projectedEyeDir.z * 0.5 + 0.5 < depthValue;
	*/
}

float4 main(VertexOut pIn) : SV_TARGET
{
	float4 diffuse;
	float3 normal;
	float4 specular;
	float3 positionW;
	float shadowFactor;
	float diffuseMultiplier;
	
	diffuse = gDiffuseTexture.Sample(samLinear, pIn.Tex);
	normal = gNormalTexture.Sample(samLinear, pIn.Tex).xyz;
	specular = gSpecularTexture.Sample(samLinear, pIn.Tex);
	positionW = gPositionTexture.Sample(samLinear, pIn.Tex).xyz;
	shadowFactor = gDiffuseTexture.Sample(samLinear, pIn.Tex).w;

	// Pretty ugly, normal texture w component contains a "diffuse multiplier"
	// which sets the inital lit diffuse color by the unlit diffuse color multiplied with
	// the diffuseMultiplier
	// (MAINLY FOR SKYBOX)
	diffuseMultiplier = gNormalTexture.Sample(samLinear, pIn.Tex).w;

	// The toEye vector is used in lighting
	float3 toEye = gEyePosW - positionW;

	// Used in shadow mapping
	//float3 eyeDir = positionW - gEyePosW;
	//eyeDir /= length(eyeDir);

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye);

	// Normalize
	toEye /= distToEye;

	float4 litColor = diffuse;

	float4 ambient_Lights = float4(0.0f, 0.0f, 0.0f, 0.0f);
	//float4 diffuse_Lights = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse_Lights = float4(diffuse.x*diffuseMultiplier, diffuse.y*diffuseMultiplier, diffuse.z*diffuseMultiplier, 0.0f);
	float4 specular_Lights = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float4 A, D, S;

	/*
	float3 shadow = float3(1.0f, 1.0f, 1.0f);
	//shadow[0] = ReadShadowMap(eyeDir);
	shadow[0] = CalcShadowFactor(samShadow, gShadowMap, pIn.shadowPosH);

	//float4 posFormLightView = mul(float4(gEyePosW, 1.0f), gCameraProj);
	//posFormLightView.xyz = posFormLightView.xyz / posFormLightView.w;
	//shadow[0] = CalcShadowFactor(samShadow, gShadowMap, posFormLightView);

	//float3 shadow = float3(1.0f, 1.0f, 1.0f);
	//shadow[0] = ReadShadowMap(eyeDir);

	//float4 shadowPosH = mul(float4(positionW, 1.0f), gShadowTransform);

	//shadow[0] = CalcShadowFactor(samShadow, gShadowMap, pIn.ShadowPosH);
	*/

	float ambient_occlusion = gSSAOTexture.Sample(samLinear, pIn.Tex).x;

	// Begin calculating lights
	for (int i = 0; i < gDirLightCount; ++i)
	{
		//ComputeDirectionalLight_Deferred(specular, gDirLights[i], normal, toEye, D, S);
		ComputeDirectionalLight_Deferred_Ambient(specular, gDirLights[i], normal, toEye, A, D, S);
		ambient_Lights += A * ambient_occlusion * shadowFactor;
		diffuse_Lights += D * shadowFactor;
		specular_Lights += S * shadowFactor;
	}

	for (int j = 0; j < gPointLightCount; ++j)
	{
		//ComputePointLight_Deferred(specular, gPointLights[j], positionW, normal, toEye, D, S);
		ComputePointLight_Deferred_Ambient(specular, gPointLights[j], positionW, normal, toEye, A, D, S);
		ambient_Lights += A * ambient_occlusion;
		diffuse_Lights += D;
		specular_Lights += S;
	}

	for (int k = 0; k < gSpotLightCount; ++k)
	{
		ComputeSpotLight_Deferred(specular, gSpotLights[k], positionW, normal, toEye, A, D, S);
		ambient_Lights += A * ambient_occlusion;
		diffuse_Lights += D;
		specular_Lights += S;
	}

	//return float4(ambient_occlusion, ambient_occlusion, ambient_occlusion, 1.0);

	//litColor = diffuse * (ambient_Lights + diffuse_Lights) + specular_Lights;
	litColor = float4(diffuse.xyz * (ambient_Lights.xyz + diffuse_Lights.xyz) + specular_Lights.xyz, 1.0f);
	//litColor *= ambient_occlusion;

	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
	//litColor.a = 1.0;
	return litColor;
}