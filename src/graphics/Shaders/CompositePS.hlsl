struct VertexOut
{
	float4 position : SV_Position;
	float2 uv : TexCoord;
};

Texture2D framebufferTexture : register(t0);
Texture2D DoFCoCTexture : register(t1);
Texture2D DoFFarFieldTexture : register(t2);

SamplerState linearClampedSampler : register(s0);

float4 main(VertexOut input) : SV_Target
{
	float3 sharp = framebufferTexture.Sample(linearClampedSampler, input.uv).xyz;
	float3 blurred = DoFFarFieldTexture.Sample(linearClampedSampler, input.uv).xyz;
	float circle_of_confusion = DoFCoCTexture.Sample(linearClampedSampler, input.uv).x;

	return float4(lerp(sharp , blurred, abs(circle_of_confusion)), 1.0);
}
