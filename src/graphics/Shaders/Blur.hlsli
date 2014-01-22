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
	const int maxCoCRadiusPixels = 10; // no idea

	const int kernel_taps = 6;
	float kernel[kernel_taps + 1];
	// 11 x 11 separated kernel weights.  This does not dictate the 
    // blur kernel for depth of field; it is scaled to the actual
    // kernel at each pixel.
	kernel[6] = 0.00000000000000;  // Weight applied to outside-radius values

    // Custom // Gaussian
	kernel[5] = 0.50;//0.04153263993208;
	kernel[4] = 0.60;//0.06352050813141;
	kernel[3] = 0.75;//0.08822292796029;
	kernel[2] = 0.90;//0.11143948794984;
	kernel[1] = 1.00;//0.12815541114232;
	kernel[0] = 1.00;//0.13425804976814;

	float2 pixel_size = float2(1.0, 1.0) / framebufferSize;
	float circleOfConfusion = inputTexture.Sample(linearClampedSampler, input.uv).x;

	float3 blur_result = float3(0.0, 0.0, 0.0);
	float weight_sum = 0.0;

	for (int i = -maxCoCRadiusPixels; i <= maxCoCRadiusPixels; ++i)	{
		float2 offset = float2(i, 0.0).DIRECTION_SWIZZLE * pixel_size;

#if defined HORIZONTAL
		float3 samppleColor = framebufferTexture.Sample(linearClampedSampler, input.uv + offset).xyz;
#elif defined VERTICAL
		// On the vertical blur pass the framebufferTexture is a half the size.
		float3 samppleColor = framebufferTexture.Sample(linearClampedSampler, input.uv + offset * 0.5).xyz;
#endif

		float samppleCoC = inputTexture.Sample(linearClampedSampler, input.uv + offset).x;

        // if (samppleCoC <= circleOfConfusion)

		float sampleRadius = sign(samppleCoC) * 3.0 + samppleCoC * max(0.0, float(maxCoCRadiusPixels) - 3.0);





		// Stretch the kernel extent to the radius at pixel B.  This implicitly 
        // performs the test of whether B's radius includes A.
        float weight = kernel[clamp(int(float(abs(i) * (kernel_taps - 1)) / (0.001 + abs(sampleRadius * 0.8))), 0, kernel_taps)];
        
        // An alternative to the branch above...this reduces some glowing, but
        // makes objects blurred against the sky disappear entirely, which creates
        // more artifacts during compositing
        // weight *= saturate(n_A - n_B + abs(n_B) * 0.2 + 0.15);

		blur_result += samppleColor * weight;
		weight_sum  += weight;
	}

	return float4(blur_result / weight_sum, 1.0);
}
#else
#error wat
#endif
