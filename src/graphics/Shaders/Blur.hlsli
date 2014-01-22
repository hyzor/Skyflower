#if defined HORIZONTAL
	#define DIRECTION_SWIZZLE xy
#elif defined VERTICAL
	#define DIRECTION_SWIZZLE yx
#else
	#error wat
#endif

struct VertexOut
{
	float4 position : SV_Position;
	float2 uv : TexCoord;
};

cbuffer cPerFramebuffer : register(b0)
{
	float2 framebufferSize;
	float z_near;
	float z_far;
};

Texture2D framebufferTexture : register(t0);
Texture2D depthTexture : register(t1);

SamplerState linearClampedSampler : register(s0);

float linear_depth(float2 uv)
{
	float depth = inputTexture.Sample(linearClampedSampler, uv).x;

	// http://www.humus.name/temp/Linearize%20depth.txt
	float c1 = z_far / z_near;
	float c0 = 1.0 - c1;

	return 1.0 / (c0 * depth + c1);
}

float main(VertexOut input) : SV_Target
{
	const int kernel_radius = 4;
	const float blur_sigma = ((float)kernel_radius + 1.0) * 0.5;
	const float blur_falloff = 1.0 / (2.0 * blur_sigma * blur_sigma);

	float2 pixel_size = float2(1.0, 1.0) / framebufferSize;
	float depth = linear_depth(input.uv);

	float result = framebufferTexture.Sample(linearClampedSampler, input.uv).x;
	float weight_sum = 1.0;

	for(float i = 1.0; i <= kernel_radius; i += 1.0) {
		float2 offset = float2(i, 0.0).DIRECTION_SWIZZLE * pixel_size;

		// Weight calculation from http://dice.se/wp-content/uploads/GDC12_Stable_SSAO_In_BF3_With_STF.pdf
		float delta_depth = depth - linear_depth(input.uv + offset);
		float weight = exp2(-i * i * blur_falloff - delta_depth * delta_depth);
		result += framebufferTexture.Sample(linearClampedSampler, input.uv + offset).x * weight;
		weight_sum += weight;

		delta_depth = depth - linear_depth(input.uv - offset);
		weight = exp2(-i * i * blur_falloff - delta_depth * delta_depth);
		result += framebufferTexture.Sample(linearClampedSampler, input.uv - offset).x * weight;
		weight_sum += weight;
	}

	return result / weight_sum;
}
