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
Texture2D inputTexture : register(t1);

SamplerState linearClampedSampler : register(s0);

#if defined SSAO_BLUR
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
#elif defined DOF_BLUR
float4 main(VertexOut input) : SV_Target
{
#if defined HORIZONTAL
	const float framebufferScale = 1.0;
#elif defined VERTICAL
	// On the vertical blur pass the framebufferTexture is a half the size.
	const float framebufferScale = 0.5;
#endif

	const int maxCoCRadiusPixels = 5;

	const int kernel_taps = 6;
	float kernel[kernel_taps + 1];
	// 11 x 11 separated kernel weights.  This does not dictate the 
    // blur kernel for depth of field; it is scaled to the actual
    // kernel at each pixel.

	// Gaussian
	kernel[0] = 0.13425804976814;
	kernel[1] = 0.12815541114232;
	kernel[2] = 0.11143948794984;
	kernel[3] = 0.08822292796029;
	kernel[4] = 0.06352050813141;
	kernel[5] = 0.04153263993208;
	// Weight applied to outside-radius values
	kernel[6] = 0.00000000000000;

	const float2 pixel_size = float2(1.0, 1.0) / framebufferSize;

	//float centerCoC = inputTexture.Sample(linearClampedSampler, input.uv).x;
	//centerCoC = centerCoC * 2.0 - 1.0;

	float3 blur_result = float3(0.0, 0.0, 0.0);
	float weight_sum = 0.0;

	for (int i = -maxCoCRadiusPixels; i <= maxCoCRadiusPixels; ++i)	{
		float2 offset = float2(i, 0.0).DIRECTION_SWIZZLE * pixel_size;

		float3 sampleColor = framebufferTexture.Sample(linearClampedSampler, input.uv + offset * framebufferScale).xyz;
		float sampleCoC = inputTexture.Sample(linearClampedSampler, input.uv + offset).x;
		sampleCoC = sampleCoC * 2.0 - 1.0;

		float sampleRadius = sign(sampleCoC) * 3.0 + sampleCoC * max(0.0, float(maxCoCRadiusPixels) - 3.0);

		// Stretch the kernel extent to the radius at pixel B.  This implicitly 
		// performs the test of whether B's radius includes A.
		float weight = kernel[clamp(int(float(abs(i) * (kernel_taps - 1)) / (0.001 + abs(sampleRadius * 0.8))), 0, kernel_taps)];
        
		blur_result += sampleColor * weight;
		weight_sum  += weight;
	}

	return float4(blur_result / weight_sum, 1.0);
}
#else
#error wat
#endif
