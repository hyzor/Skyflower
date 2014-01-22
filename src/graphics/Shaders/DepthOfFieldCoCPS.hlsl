struct VertexOut
{
	float4 position : SV_Position;
	float2 uv : TexCoord;
};

cbuffer cPerFramebuffer : register(b0)
{
	float z_near;
	float z_far;
	float near_blurry_plane;
	float near_sharp_plane;
	float far_sharp_plane;
	float far_blurry_plane;
	float near_scale;
	float far_scale;
};

Texture2D depthTexture : register(t0);

SamplerState linearSampler : register(s0);

float linear_depth(float2 uv)
{
	float depth = depthTexture.Sample(linearSampler, uv).x;

	// http://www.humus.name/temp/Linearize%20depth.txt
	float c1 = z_far / z_near;
	float c0 = 1.0 - c1;

	return 1.0 / (c0 * depth + c1);
}

float main(VertexOut input) : SV_Target
{
	const float scale = 1.0;
	const float bias = 0.0;

	float depth = linear_depth(input.uv) * (z_far - z_near);
	float radius = 0.0;

	if (depth < near_sharp_plane) {
		// In the near field , produce a negative value
		radius = (max(depth, near_blurry_plane) - near_sharp_plane) * near_scale;
	}
	else if (depth > far_sharp_plane) {
		// In the far field , produce a positive value
		radius = (min(depth, far_blurry_plane) - far_sharp_plane) * far_scale;
	}

	// The skybox should always be in the focus field.
	if (depth > 1500.0) {
		radius = 0.0;
	}

	float result = radius * scale + bias;
	result = (result + 1.0) * 0.5;

	return result;
}
