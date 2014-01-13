struct VertexOut
{
	float4 position : SV_Position;
	float2 uv : TexCoord;
};

cbuffer cPerFramebuffer : register(b0)
{
	float4x4 inverseProjectionMatrix;
	float4x4 viewMatrix;
	float z_far;
};

Texture2D depthTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D randomTexture : register(t2);

SamplerState linearClampedSampler : register(s0);
SamplerState linearSampler : register(s1);

float3 reconstruct_viewspace_position(float2 uv)
{
	float depth = depthTexture.SampleLevel(linearClampedSampler, uv, 0).x;

	float3 ndc_position;
	ndc_position.x = uv.x * 2.0 - 1.0;
	ndc_position.y = (1.0 - uv.y) * 2.0 - 1.0;
	// In D3D NDC the near plane is clipped to 0 and not -1.
	ndc_position.z = depth;

	float4 position = mul(float4(ndc_position, 1.0), inverseProjectionMatrix);

	return position.xyz / position.w;
}

// http://graphics.cs.williams.edu/papers/AlchemyHPG11/
// http://graphics.cs.williams.edu/papers/SAOHPG12/

float4 main(VertexOut input) : SV_Target
{
	float3 position = reconstruct_viewspace_position(input.uv);
	float3 normal = normalTexture.Sample(linearClampedSampler, input.uv).xyz;
	// Convert from worldspace to viewspace.
	normal = normalize(mul(float4(normal, 0.0), viewMatrix).xyz);

	const float radius = 0.25;
	const float projection_factor = 0.75;
	const float random_offset = 18.0;
	const float epsilon = 0.0001;
	/* Bias distance is the ambient-obscurance analog of shadow map
	 * bias: increase to reduce self-shadowing (figure 4), decrease if
	 * light leaks into corners. We scale it by z (which is negative) to
	 * efficiently compensate for the dot products becoming increasingly
	 * sensitive to error in normal at distant points.
	 */
	const float bias = 0.0001;

	const int base_samples = 16;
	const int min_samples = 4;
	// Decrease number of samples with increasing range.
	int samples = max(int(base_samples / (1.0 + base_samples * (position.z / z_far))), min_samples);

	float projected_radius = projection_factor * radius / position.z;
	float ambient_occlusion = 0.0;

	for (int i = 0; i < samples; ++i) {
		float2 random_direction = randomTexture.SampleLevel(linearSampler, input.uv * random_offset, 0).xy;
		random_direction = normalize(random_direction * 2.0 - 1.0);

		float3 occlusion_sample = reconstruct_viewspace_position(input.uv + random_direction * projected_radius);
		float3 v = normalize(occlusion_sample - position);

		// FIXME: Should we use position.z or occlusion_sample.z?
		// FIXME: Do we want to subtract or add bias?
		ambient_occlusion += max(0.0, dot(v, normal) - position.z * bias) / (dot(v, v) + epsilon);
	}

	const float k = 1.0;
	const float sigma = 1.0;
	ambient_occlusion = pow(max(0.0, 1.0 - 2.0 * sigma / float(samples) * ambient_occlusion), k);

	return float4(ambient_occlusion, ambient_occlusion, ambient_occlusion, 1.0);
}
