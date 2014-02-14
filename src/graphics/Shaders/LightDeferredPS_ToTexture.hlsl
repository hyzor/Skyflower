#include "LightDeferredVS.hlsl"
#include "LightDef.hlsli"

//http://frictionalgames.blogspot.se/2012/09/tech-feature-hdr-lightning.html
float3 Uncharted2Tonemap(float3 x)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;

	return ((x*(A*x + C*B) + D*E) / (x*(A*x + B) + D*F)) - E / F;
};

#define MAX_MOTIONBLURSAMPLES 32

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
	float gFogHeightFalloff, gFogHeightOffset, gFogGlobalDensity;

	float4 gFogColor;

	int gEnableMotionBlur;
	float gCurFps;
	float gTargetFps;
	int padding001;

	// Needs cleanup!
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
Texture2D gVelocityTexture : register(t3);
Texture2D gSSAOTexture : register(t4);
Texture2D gDepthTexture : register(t5);
Texture2D gBackgroundTexture : register(t6);

SamplerState samLinear : register(s0);
SamplerState samAnisotropic : register(s1);
SamplerComparisonState samShadow : register(s2);
SamplerState samPoint : register(s3);

struct PixelOut
{
	float4 LitColor : SV_Target0;
};

PixelOut main(VertexOut pIn)
{
	PixelOut pOut;

	float4 diffuse;
	float3 normal;
	float4 specular;
	float3 positionW;
	float shadowFactor;
	float diffuseMultiplier;
	float2 velocity;
	
	diffuse = gDiffuseTexture.Sample(samLinear, pIn.Tex);
	normal = gNormalTexture.Sample(samLinear, pIn.Tex).xyz;
	specular = gSpecularTexture.Sample(samLinear, pIn.Tex);
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

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye);

	// Normalize
	toEye /= distToEye;

	//--------------------------------------------------
	// Motion blur
	//--------------------------------------------------
	// http://http.developer.nvidia.com/GPUGems3/gpugems3_ch27.html
	// Microsoft DirectX SDK (June 2010)\Samples\C++\Direct3D\PixelMotionBlur
	// http://john-chapman-graphics.blogspot.se/2013/01/per-object-motion-blur.html

	if (gEnableMotionBlur)
	{
		velocity = gVelocityTexture.Sample(samPoint, pIn.Tex).xy;
		velocity = pow(velocity, 1.0f / 3.0f);
		velocity = velocity * 2.0f - 1.0f;

		// Velocity scales with the current fps
		float velocityScale = gCurFps / gTargetFps;
		velocity *= velocityScale;

		// Get velocity texture dimensions
		// (This could be sent in to the shader from the outside instead of calculating every time)
		float2 velocityDimensions;
		gVelocityTexture.GetDimensions(velocityDimensions.x, velocityDimensions.y);
		float2 texelSize = 1.0f / velocityDimensions;

		// Figure out how many samples to take
		float speed = length(velocity / texelSize);
		unsigned int numMotionBlurSamples = clamp(int(speed), 1, MAX_MOTIONBLURSAMPLES);

		[unroll]
		for (unsigned int n = 1; n < numMotionBlurSamples; ++n)
		{
			float2 offset = velocity * (float(n) / float(numMotionBlurSamples - 1) - 0.5f);
			diffuse.xyz += gDiffuseTexture.Sample(samPoint, pIn.Tex + offset).xyz;
		}

		diffuse.xyz /= float(numMotionBlurSamples);
	}

	//--------------------------------------------------
	// Lighting
	//--------------------------------------------------
	//float4 litColor = diffuse;
	pOut.LitColor = diffuse;

	float4 ambient_Lights = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse_Lights = float4(diffuse.x*diffuseMultiplier, diffuse.y*diffuseMultiplier, diffuse.z*diffuseMultiplier, 0.0f);
	float4 specular_Lights = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float4 A, D, S;

	float ambient_occlusion = gSSAOTexture.Sample(samLinear, pIn.Tex).x;

	// Begin calculating lights
	for (int i = 0; i < gDirLightCount; ++i)
	{
		ComputeDirectionalLight_Deferred_Ambient(specular, gDirLights[i], normal, toEye, A, D, S);
		ambient_Lights += A * ambient_occlusion * shadowFactor;
		diffuse_Lights += D * shadowFactor;
		specular_Lights += S * shadowFactor;
	}

	for (int j = 0; j < gPointLightCount; ++j)
	{
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

	pOut.LitColor = float4(diffuse.xyz * (ambient_Lights.xyz + diffuse_Lights.xyz) + specular_Lights.xyz, 1.0f);

	//--------------------------------------------------
	// Fogging
	//--------------------------------------------------
	// http://developer.amd.com/wordpress/media/2012/10/Wenzel-Real-time_Atmospheric_Effects_in_Games.pdf
	// http://udn.epicgames.com/Three/HeightFog.html
	// http://iancubin.wordpress.com/projects/opengl-volumetric-fog/
	// http://msdn.microsoft.com/en-us/library/bb173401%28v=vs.85%29.aspx

	if (gEnableFogging)
	{
		float3 eyeDirOffset = eyeDir;
		eyeDirOffset.y -= gFogHeightOffset;

		float cVolFogHeightDensityAtViewer = exp(-gFogHeightFalloff * (gEyePosW.y - gFogHeightOffset));
		float fogIntensity = length(eyeDirOffset) * cVolFogHeightDensityAtViewer;

		const float slopeThreshold = 0.01f;

		if (abs(eyeDirOffset.y - gFogHeightOffset) > slopeThreshold)
		{
			float t = gFogHeightFalloff * (eyeDirOffset.y - gFogHeightOffset);
			fogIntensity *= (1.0f - exp(-t)) / t;
		}

		float finalIntensity = exp(-gFogGlobalDensity * fogIntensity);

		pOut.LitColor = finalIntensity * pOut.LitColor + (1 - finalIntensity) * gFogColor;
	}

	//return litColor;

	// Exposure and vignetting
	//float luminance = dot(diffuse.xyz, float3(0.333f, 0.333f, 0.333f));

	/*
	float exposure = 1.25f;
	float2 vtc = float2(pIn.Tex - 0.5);
	float vignette = pow(1 - (dot(vtc, vtc) * 1.0), 2.0);

	float3 exposed = 1.0 - pow(2.71, -(vignette * pOut.LitColor.xyz * exposure));

	pOut.LitColor.xyz = exposed;
	*/

	// Tone mapping
	//pOut.LitColor.xyz = Uncharted2Tonemap(pOut.LitColor.xyz);

	float4 backgroundColor = gBackgroundTexture.Sample(samLinear, pIn.Tex);

		float3 colorOut = pOut.LitColor.xyz * float3(1.0f, 1.0f, 1.0f) + backgroundColor.xyz * float3(1.0f, 1.0f, 1.0f);

		pOut.LitColor.xyz = colorOut.xyz;

	// Gamma encode final lit color
	pOut.LitColor.xyz = pow(pOut.LitColor.xyz, 1.0f / 2.2f);

	return pOut;
}