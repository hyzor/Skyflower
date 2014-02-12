#include "SMAA/SMAA_Shared.hlsli"

Texture2D edgesTex : register(t0);
Texture2D areaTex : register(t1);
Texture2D searchTex : register(t2);

cbuffer cbPerFrame : register(b0)
{
	unsigned int screenWidth;
	unsigned int screenHeight;
	float2 padding;
};

struct VertexIn
{
	float2 texCoord : TEXCOORD;
	float2 pixCoord : PIXCOORD;
	float4 offset[3] : OFFSET;
	float4 subSampleIndices : SAMPLEINDICES;
};

float4 main(VertexIn vIn) : SV_TARGET
{
	float4 SMAA_RT_METRICS = float4(1.0 / screenWidth, 1.0 / screenHeight, screenWidth, screenHeight);

	float4 weights = float4(0.0, 0.0, 0.0, 0.0);

	float2 e = SMAASample(edgesTex, vIn.texCoord).rg;

	SMAA_BRANCH
	if (e.g > 0.0) { // Edge at north
#if !defined(SMAA_DISABLE_DIAG_DETECTION)
		// Diagonals have both north and west edges, so searching for them in
		// one of the boundaries is enough.
		weights.rg = SMAACalculateDiagWeights(SMAATexturePass2D(edgesTex), SMAATexturePass2D(areaTex), vIn.texCoord, e, vIn.subSampleIndices);

		// We give priority to diagonals, so if we find a diagonal we skip 
		// horizontal/vertical processing.
		SMAA_BRANCH
			if (weights.r == -weights.g) { // weights.r + weights.g == 0.0
#endif

				float2 d;

				// Find the distance to the left:
				float3 coords;
				coords.x = SMAASearchXLeft(SMAATexturePass2D(edgesTex), SMAATexturePass2D(searchTex), vIn.offset[0].xy, vIn.offset[2].x, SMAA_RT_METRICS);
				coords.y = vIn.offset[1].y; // vIn.offset[1].y = texcoord.y - 0.25 * SMAA_RT_METRICS.y (@CROSSING_OFFSET)
				d.x = coords.x;

				// Now fetch the left crossing edges, two at a time using bilinear
				// filtering. Sampling at -0.25 (see @CROSSING_OFFSET) enables to
				// discern what value each edge has:
				float e1 = SMAASampleLevelZero(edgesTex, coords.xy).r;

				// Find the distance to the right:
				coords.z = SMAASearchXRight(SMAATexturePass2D(edgesTex), SMAATexturePass2D(searchTex), vIn.offset[0].zw, vIn.offset[2].y, SMAA_RT_METRICS);
				d.y = coords.z;

				// We want the distances to be in pixel units (doing this here allow to
				// better interleave arithmetic and memory accesses):
				d = abs(round(mad(SMAA_RT_METRICS.zz, d, -vIn.pixCoord.xx)));

				// SMAAArea below needs a sqrt, as the areas texture is compressed
				// quadratically:
				float2 sqrt_d = sqrt(d);

				// Fetch the right crossing edges:
				float e2 = SMAASampleLevelZeroOffset(edgesTex, coords.zy, int2(1, 0)).r;

				// Ok, we know how this pattern looks like, now it is time for getting
				// the actual area:
				weights.rg = SMAAArea(SMAATexturePass2D(areaTex), sqrt_d, e1, e2, vIn.subSampleIndices.y);

				// Fix corners:
				coords.y = vIn.texCoord.y;
				SMAADetectHorizontalCornerPattern(SMAATexturePass2D(edgesTex), weights.rg, coords.xyzy, d);

#if !defined(SMAA_DISABLE_DIAG_DETECTION)
			}
			else
				e.r = 0.0; // Skip vertical processing.
#endif
	}

	SMAA_BRANCH
		if (e.r > 0.0) { // Edge at west
			float2 d;

			// Find the distance to the top:
			float3 coords;
			coords.y = SMAASearchYUp(SMAATexturePass2D(edgesTex), SMAATexturePass2D(searchTex), vIn.offset[1].xy, vIn.offset[2].z, SMAA_RT_METRICS);
			coords.x = vIn.offset[0].x; // vIn.offset[1].x = texcoord.x - 0.25 * SMAA_RT_METRICS.x;
			d.x = coords.y;

			// Fetch the top crossing edges:
			float e1 = SMAASampleLevelZero(edgesTex, coords.xy).g;

			// Find the distance to the bottom:
			coords.z = SMAASearchYDown(SMAATexturePass2D(edgesTex), SMAATexturePass2D(searchTex), vIn.offset[1].zw, vIn.offset[2].w, SMAA_RT_METRICS);
			d.y = coords.z;

			// We want the distances to be in pixel units:
			d = abs(round(mad(SMAA_RT_METRICS.ww, d, -vIn.pixCoord.yy)));

			// SMAAArea below needs a sqrt, as the areas texture is compressed 
			// quadratically:
			float2 sqrt_d = sqrt(d);

				// Fetch the bottom crossing edges:
				float e2 = SMAASampleLevelZeroOffset(edgesTex, coords.xz, int2(0, 1)).g;

			// Get the area for this direction:
			weights.ba = SMAAArea(SMAATexturePass2D(areaTex), sqrt_d, e1, e2, vIn.subSampleIndices.x);

			// Fix corners:
			coords.x = vIn.texCoord.x;
			SMAADetectVerticalCornerPattern(SMAATexturePass2D(edgesTex), weights.ba, coords.xyxz, d);
		}

	return weights;
}