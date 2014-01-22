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

	int gEnableFogging;
	float gFogRange, gFogStart;
	int fogPadding;

	float4 gFogColor;

	float4x4 gShadowTransform_PS; // Light: view * projection * toTexSpace
	float4x4 gCameraView;
	float4x4 gCameraInvView;
	float4x4 gCameraWorld;
	float4x4 gCameraProj;
	float4x4 gCamViewProjInv;
	float4x4 gLightWorld;
	float4x4 gLightView;
	float4x4 gLightInvView;
	float4x4 gLightProj;
};

Texture2D gDiffuseTexture : register(t0);
Texture2D gNormalTexture : register(t1);
Texture2D gSpecularTexture : register(t2);
//Texture2D gPositionTexture : register(t3);
Texture2D gSSAOTexture : register(t4);
Texture2D gDepthTexture : register(t5);
//Texture2D gShadowMap : register(t6);

SamplerState samLinear : register(s0);
SamplerState samAnisotropic : register(s1);
SamplerComparisonState samShadow : register(s2);

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
	//positionW = gPositionTexture.Sample(samLinear, pIn.Tex).xyz;
	shadowFactor = gDiffuseTexture.Sample(samLinear, pIn.Tex).w;

	// Pretty ugly, normal texture w component contains a "diffuse multiplier"
	// which sets the inital lit diffuse color by the unlit diffuse color multiplied with
	// the diffuseMultiplier
	// (MAINLY FOR SKYBOX)
	diffuseMultiplier = gNormalTexture.Sample(samLinear, pIn.Tex).w;

	// World pos reconstruction
	float depth = gDepthTexture.Sample(samLinear, pIn.Tex).x;

	float4 H = float4(pIn.Tex.x * 2.0f - 1.0f, (1.0f - pIn.Tex.y) * 2.0f - 1.0f, depth, 1.0f);

	float4 D_transformed = mul(H, gCamViewProjInv);

	positionW = (D_transformed / D_transformed.w).xyz;

	// The toEye vector is used in lighting
	float3 toEye = gEyePosW - positionW;

	// Direction of eye to pixel world position
	float3 eyeDir = positionW - gEyePosW;
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

	litColor = float4(diffuse.xyz * (ambient_Lights.xyz + diffuse_Lights.xyz) + specular_Lights.xyz, 1.0f);

	//--------------------------------------------------
	// Fogging
	//--------------------------------------------------
	// http://developer.amd.com/wordpress/media/2012/10/Wenzel-Real-time_Atmospheric_Effects_in_Games.pdf
	// http://udn.epicgames.com/Three/HeightFog.html
	// http://iancubin.wordpress.com/projects/opengl-volumetric-fog/
	// http://msdn.microsoft.com/en-us/library/bb173401%28v=vs.85%29.aspx
	if (gEnableFogging)
	{
		float heightFalloff = 0.0075f; // Fog fade speed
		float globalDensity = 0.005f;
		float fogOffset = -100.0f; // Offset fog height

		float3 eyeDirOffset = eyeDir;
		eyeDirOffset.y -= fogOffset;

		float cVolFogHeightDensityAtViewer = exp(-heightFalloff * (gEyePosW.y - fogOffset));
		float fogInt = length(eyeDirOffset) * cVolFogHeightDensityAtViewer;

		const float slopeThreshold = 0.01f;

		if (abs(eyeDir.y - fogOffset) > slopeThreshold)
		{
			float t = heightFalloff * (eyeDir.y - fogOffset);
			fogInt *= (1.0f - exp(-t)) / t;
		}

		float finalInt = exp(-globalDensity * fogInt);

		litColor = finalInt * litColor + (1 - finalInt) * gFogColor;
	}

	return litColor;
}