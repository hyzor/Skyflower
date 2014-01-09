struct VertexOut
{
	float4 position : SV_Position;
	float2 uv : TexCoord;
};

cbuffer cPerFramebuffer : register(b0)
{
	float3 eyePos;
	float z_far;
	float2 framebufferSize;

	float4x4 projectionMatrix;
	float4x4 viewProjectionMatrix;
	float4x4 inverseViewProjectionMatrix;
};

Texture2D depthTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D randomTexture : register(t2);

SamplerState linearSampler : register(s0);

float4 main(VertexOut input) : SV_Target
{
	float3 normal = normalTexture.Sample(linearSampler, input.uv).rgb;

	return float4(normal, 1.0);
}
