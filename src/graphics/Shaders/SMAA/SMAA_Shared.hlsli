//-----------------------------------------------------------------------------
// SMAA Presets
//-----------------------------------------------------------------------------

#define SMAA_PRESET_MEDIUM = 1

#if defined(SMAA_PRESET_LOW)
#define SMAA_THRESHOLD 0.15
#define SMAA_MAX_SEARCH_STEPS 4
#define SMAA_DISABLE_DIAG_DETECTION
#define SMAA_DISABLE_CORNER_DETECTION
#elif defined(SMAA_PRESET_MEDIUM)
#define SMAA_THRESHOLD 0.1
#define SMAA_MAX_SEARCH_STEPS 8
#define SMAA_DISABLE_DIAG_DETECTION
#define SMAA_DISABLE_CORNER_DETECTION
#elif defined(SMAA_PRESET_HIGH)
#define SMAA_THRESHOLD 0.1
#define SMAA_MAX_SEARCH_STEPS 16
#define SMAA_MAX_SEARCH_STEPS_DIAG 8
#define SMAA_CORNER_ROUNDING 25
#elif defined(SMAA_PRESET_ULTRA)
#define SMAA_THRESHOLD 0.05
#define SMAA_MAX_SEARCH_STEPS 32
#define SMAA_MAX_SEARCH_STEPS_DIAG 16
#define SMAA_CORNER_ROUNDING 25
#endif

//-----------------------------------------------------------------------------
// Configurable Defines
//-----------------------------------------------------------------------------

/**
* SMAA_THRESHOLD specifies the threshold or sensitivity to edges.
* Lowering this value you will be able to detect more edges at the expense of
* performance.
*
* Range: [0, 0.5]
*   0.1 is a reasonable value, and allows to catch most visible edges.
*   0.05 is a rather overkill value, that allows to catch 'em all.
*
*   If temporal supersampling is used, 0.2 could be a reasonable value, as low
*   contrast edges are properly filtered by just 2x.
*/
#ifndef SMAA_THRESHOLD
#define SMAA_THRESHOLD 0.1
#endif

/**
* Threshold to be used in the additional predication buffer.
*
* Range: depends on the input, so you'll have to find the magic number that
* works for you.
*/
#ifndef SMAA_PREDICATION_THRESHOLD
#define SMAA_PREDICATION_THRESHOLD 0.01
#endif

/**
* How much to scale the global threshold used for luma or color edge
* detection when using predication.
*
* Range: [1, 5]
*/
#ifndef SMAA_PREDICATION_SCALE
#define SMAA_PREDICATION_SCALE 2.0
#endif

/**
* How much to locally decrease the threshold.
*
* Range: [0, 1]
*/
#ifndef SMAA_PREDICATION_STRENGTH
#define SMAA_PREDICATION_STRENGTH 0.4
#endif

/**
* If there is an neighbor edge that has SMAA_LOCAL_CONTRAST_FACTOR times
* bigger contrast than current edge, current edge will be discarded.
*
* This allows to eliminate spurious crossing edges, and is based on the fact
* that, if there is too much contrast in a direction, that will hide
* perceptually contrast in the other neighbors.
*/
#ifndef SMAA_LOCAL_CONTRAST_ADAPTATION_FACTOR
#define SMAA_LOCAL_CONTRAST_ADAPTATION_FACTOR 2.0
#endif

/**
* Predicated thresholding allows to better preserve texture details and to
* improve performance, by decreasing the number of detected edges using an
* additional buffer like the light accumulation buffer, object ids or even the
* depth buffer (the depth buffer usage may be limited to indoor or short range
* scenes).
*
* It locally decreases the luma or color threshold if an edge is found in an
* additional buffer (so the global threshold can be higher).
*
* This method was developed by Playstation EDGE MLAA team, and used in
* Killzone 3, by using the light accumulation buffer. More information here:
*     http://iryoku.com/aacourse/downloads/06-MLAA-on-PS3.pptx
*/
#ifndef SMAA_PREDICATION
#define SMAA_PREDICATION 0
#endif

/**
* SMAA_DEPTH_THRESHOLD specifies the threshold for depth edge detection.
*
* Range: depends on the depth range of the scene.
*/
#ifndef SMAA_DEPTH_THRESHOLD
#define SMAA_DEPTH_THRESHOLD (0.1 * SMAA_THRESHOLD)
#endif

//-----------------------------------------------------------------------------
// Texture Access Defines
//-----------------------------------------------------------------------------

#ifndef SMAA_AREATEX_SELECT
#if defined(SMAA_HLSL_3)
#define SMAA_AREATEX_SELECT(sample) sample.ra
#else
#define SMAA_AREATEX_SELECT(sample) sample.rg
#endif
#endif

#ifndef SMAA_SEARCHTEX_SELECT
#define SMAA_SEARCHTEX_SELECT(sample) sample.r
#endif

#ifndef SMAA_DECODE_VELOCITY
#define SMAA_DECODE_VELOCITY(sample) sample.rg
#endif

//-----------------------------------------------------------------------------
// Non-Configurable Defines
//-----------------------------------------------------------------------------

#define SMAA_AREATEX_MAX_DISTANCE 16
#define SMAA_AREATEX_MAX_DISTANCE_DIAG 20
#define SMAA_AREATEX_PIXEL_SIZE (1.0 / float2(160.0, 560.0))
#define SMAA_AREATEX_SUBTEX_SIZE (1.0 / 7.0)
#define SMAA_SEARCHTEX_SIZE float2(66.0, 33.0)
#define SMAA_SEARCHTEX_PACKED_SIZE float2(64.0, 16.0)
#define SMAA_CORNER_ROUNDING_NORM (float(SMAA_CORNER_ROUNDING) / 100.0)

//-----------------------------------------------------------------------------
// Misc functions
//-----------------------------------------------------------------------------

SamplerState LinearSampler { Filter = MIN_MAG_LINEAR_MIP_POINT; AddressU = Clamp; AddressV = Clamp; };
SamplerState PointSampler { Filter = MIN_MAG_MIP_POINT; AddressU = Clamp; AddressV = Clamp; };
#define SMAATexture2D(tex) Texture2D tex
#define SMAATexturePass2D(tex) tex
#define SMAASampleLevelZero(tex, coord) tex.SampleLevel(LinearSampler, coord, 0)
#define SMAASampleLevelZeroPoint(tex, coord) tex.SampleLevel(PointSampler, coord, 0)
#define SMAASampleLevelZeroOffset(tex, coord, offset) tex.SampleLevel(LinearSampler, coord, 0, offset)
#define SMAASample(tex, coord) tex.Sample(LinearSampler, coord)
#define SMAASamplePoint(tex, coord) tex.Sample(PointSampler, coord)
#define SMAASampleOffset(tex, coord, offset) tex.Sample(LinearSampler, coord, offset)
#define SMAA_FLATTEN [flatten]
#define SMAA_BRANCH [branch]
#define SMAATexture2DMS2(tex) Texture2DMS<float4, 2> tex
#define SMAALoad(tex, pos, sample) tex.Load(pos, sample)
#define SMAAGather(tex, coord) tex.Gather(LinearSampler, coord, 0)

/**
* Gathers current pixel, and the top-left neighbors.
*/
float3 SMAAGatherNeighbours(float2 texcoord,
	float4 offset[3],
	Texture2D tex,
	unsigned int screenWidth,
	unsigned int screenHeight)
{
	float4 SMAA_RT_METRICS = float4(1.0 / screenWidth, 1.0 / screenHeight, screenWidth, screenHeight);

		return SMAAGather(tex, texcoord + SMAA_RT_METRICS.xy * float2(-0.5, -0.5)).grb;
}

/**
* Adjusts the threshold by means of predication.
*/
float2 SMAACalculatePredicatedThreshold(float2 texcoord,
	float4 offset[3],
	Texture2D colorTex,
	SMAATexture2D(predicationTex),
	unsigned int screenWidth,
	unsigned int screenHeight)
{
	float3 neighbours = SMAAGatherNeighbours(texcoord, offset, SMAATexturePass2D(predicationTex), screenWidth, screenHeight);
		float2 delta = abs(neighbours.xx - neighbours.yz);
		float2 edges = step(SMAA_PREDICATION_THRESHOLD, delta);
		return SMAA_PREDICATION_SCALE * SMAA_THRESHOLD * (1.0 - SMAA_PREDICATION_STRENGTH * edges);
}

/**
* Conditional move:
*/
void SMAAMovc(bool2 cond, inout float2 variable, float2 value)
{
	SMAA_FLATTEN if (cond.x) variable.x = value.x;
	SMAA_FLATTEN if (cond.y) variable.y = value.y;
}

void SMAAMovc(bool4 cond, inout float4 variable, float4 value)
{
	SMAAMovc(cond.xy, variable.xy, value.xy);
	SMAAMovc(cond.zw, variable.zw, value.zw);
}

//-----------------------------------------------------------------------------
// Diagonal Search Functions
//-----------------------------------------------------------------------------

#if !defined(SMAA_DISABLE_DIAG_DETECTION)

/**
* Allows to decode two binary values from a bilinear-filtered access.
*/
float2 SMAADecodeDiagBilinearAccess(float2 e) {
	// Bilinear access for fetching 'e' have a 0.25 offset, and we are
	// interested in the R and G edges:
	//
	// +---G---+-------+
	// |   x o R   x   |
	// +-------+-------+
	//
	// Then, if one of these edge is enabled:
	//   Red:   (0.75 * X + 0.25 * 1) => 0.25 or 1.0
	//   Green: (0.75 * 1 + 0.25 * X) => 0.75 or 1.0
	//
	// This function will unpack the values (mad + mul + round):
	// wolframalpha.com: round(x * abs(5 * x - 5 * 0.75)) plot 0 to 1
	e.r = e.r * abs(5.0 * e.r - 5.0 * 0.75);
	return round(e);
}

float4 SMAADecodeDiagBilinearAccess(float4 e) {
	e.rb = e.rb * abs(5.0 * e.rb - 5.0 * 0.75);
	return round(e);
}

/**
* These functions allows to perform diagonal pattern searches.
*/
float2 SMAASearchDiag1(SMAATexture2D(edgesTex), 
	float2 texcoord, float2 dir, float4 SMAA_RT_METRICS, out float2 e) 
{
	float4 coord = float4(texcoord, -1.0, 1.0);
	float3 t = float3(SMAA_RT_METRICS.xy, 1.0);

	while (coord.z < float(SMAA_MAX_SEARCH_STEPS_DIAG - 1) && coord.w > 0.9) 
	{
		coord.xyz = mad(t, float3(dir, 1.0), coord.xyz);
		e = SMAASampleLevelZero(edgesTex, coord.xy).rg;
		coord.w = dot(e, float2(0.5, 0.5));
	}

	return coord.zw;
}

float2 SMAASearchDiag2(SMAATexture2D(edgesTex),
	float2 texcoord, float2 dir, float4 SMAA_RT_METRICS, out float2 e)
{
	float4 coord = float4(texcoord, -1.0, 1.0);
	coord.x += 0.25 * SMAA_RT_METRICS.x; // See @SearchDiag2Optimization
	float3 t = float3(SMAA_RT_METRICS.xy, 1.0);

	while (coord.z < float(SMAA_MAX_SEARCH_STEPS_DIAG - 1) && coord.w > 0.9) 
	{
		coord.xyz = mad(t, float3(dir, 1.0), coord.xyz);

		// @SearchDiag2Optimization
		// Fetch both edges at once using bilinear filtering:
		e = SMAASampleLevelZero(edgesTex, coord.xy).rg;
		e = SMAADecodeDiagBilinearAccess(e);

		// Non-optimized version:
		// e.g = SMAASampleLevelZero(edgesTex, coord.xy).g;
		// e.r = SMAASampleLevelZeroOffset(edgesTex, coord.xy, int2(1, 0)).r;

		coord.w = dot(e, float2(0.5, 0.5));
	}

	return coord.zw;
}

/**
* Similar to SMAAArea, this calculates the area corresponding to a certain
* diagonal distance and crossing edges 'e'.
*/
float2 SMAAAreaDiag(SMAATexture2D(areaTex), float2 dist, float2 e, float offset) {
	float2 texcoord = mad(float2(SMAA_AREATEX_MAX_DISTANCE_DIAG, SMAA_AREATEX_MAX_DISTANCE_DIAG), e, dist);

		// We do a scale and bias for mapping to texel space:
		texcoord = mad(SMAA_AREATEX_PIXEL_SIZE, texcoord, 0.5 * SMAA_AREATEX_PIXEL_SIZE);

	// Diagonal areas are on the second half of the texture:
	texcoord.x += 0.5;

	// Move to proper place, according to the subpixel offset:
	texcoord.y += SMAA_AREATEX_SUBTEX_SIZE * offset;

	// Do it!
	return SMAA_AREATEX_SELECT(SMAASampleLevelZero(areaTex, texcoord));
}

/**
* This searches for diagonal patterns and returns the corresponding weights.
*/
float2 SMAACalculateDiagWeights(SMAATexture2D(edgesTex), SMAATexture2D(areaTex),
	float2 texcoord, float2 e, float4 subsampleIndices, float4 SMAA_RT_METRICS) 
{

	float2 weights = float2(0.0, 0.0);

	// Search for the line ends:
	float4 d;
	float2 end;
	if (e.r > 0.0) 
	{
		d.xz = SMAASearchDiag1(SMAATexturePass2D(edgesTex), texcoord, float2(-1.0, 1.0), end);
		d.x += float(end.y > 0.9);
	}
	else
		d.xz = float2(0.0, 0.0);
	d.yw = SMAASearchDiag1(SMAATexturePass2D(edgesTex), texcoord, float2(1.0, -1.0), end);

	SMAA_BRANCH
		if (d.x + d.y > 2.0) // d.x + d.y + 1 > 3
		{
			// Fetch the crossing edges:
			float4 coords = mad(float4(-d.x + 0.25, d.x, d.y, -d.y - 0.25), SMAA_RT_METRICS.xyxy, texcoord.xyxy);
			float4 c;
			c.xy = SMAASampleLevelZeroOffset(edgesTex, coords.xy, int2(-1, 0)).rg;
			c.zw = SMAASampleLevelZeroOffset(edgesTex, coords.zw, int2(1, 0)).rg;
			c.yxwz = SMAADecodeDiagBilinearAccess(c.xyzw);

			// Non-optimized version:
			// float4 coords = mad(float4(-d.x, d.x, d.y, -d.y), SMAA_RT_METRICS.xyxy, texcoord.xyxy);
			// float4 c;
			// c.x = SMAASampleLevelZeroOffset(edgesTex, coords.xy, int2(-1,  0)).g;
			// c.y = SMAASampleLevelZeroOffset(edgesTex, coords.xy, int2( 0,  0)).r;
			// c.z = SMAASampleLevelZeroOffset(edgesTex, coords.zw, int2( 1,  0)).g;
			// c.w = SMAASampleLevelZeroOffset(edgesTex, coords.zw, int2( 1, -1)).r;

			// Merge crossing edges at each side into a single value:
			float2 cc = mad(float2(2.0, 2.0), c.xz, c.yw);

			// Remove the crossing edge if we didn't found the end of the line:
			SMAAMovc(bool2(step(0.9, d.zw)), cc, float2(0.0, 0.0));

			// Fetch the areas for this line:
			weights += SMAAAreaDiag(SMAATexturePass2D(areaTex), d.xy, cc, subsampleIndices.z);
		}

	// Search for the line ends:
	d.xz = SMAASearchDiag2(SMAATexturePass2D(edgesTex), texcoord, float2(-1.0, -1.0), end);
	if (SMAASampleLevelZeroOffset(edgesTex, texcoord, int2(1, 0)).r > 0.0) {
		d.yw = SMAASearchDiag2(SMAATexturePass2D(edgesTex), texcoord, float2(1.0, 1.0), end);
		d.y += float(end.y > 0.9);
	}
	else
		d.yw = float2(0.0, 0.0);

	SMAA_BRANCH
		if (d.x + d.y > 2.0) { // d.x + d.y + 1 > 3
			// Fetch the crossing edges:
			float4 coords = mad(float4(-d.x, -d.x, d.y, d.y), SMAA_RT_METRICS.xyxy, texcoord.xyxy);
				float4 c;
			c.x = SMAASampleLevelZeroOffset(edgesTex, coords.xy, int2(-1, 0)).g;
			c.y = SMAASampleLevelZeroOffset(edgesTex, coords.xy, int2(0, -1)).r;
			c.zw = SMAASampleLevelZeroOffset(edgesTex, coords.zw, int2(1, 0)).gr;
			float2 cc = mad(float2(2.0, 2.0), c.xz, c.yw);

				// Remove the crossing edge if we didn't found the end of the line:
				SMAAMovc(bool2(step(0.9, d.zw)), cc, float2(0.0, 0.0));

			// Fetch the areas for this line:
			weights += SMAAAreaDiag(SMAATexturePass2D(areaTex), d.xy, cc, subsampleIndices.w).gr;
		}

	return weights;
}
#endif

//-----------------------------------------------------------------------------
// Horizontal/Vertical Search Functions
//-----------------------------------------------------------------------------

/**
* This allows to determine how much length should we add in the last step
* of the searches. It takes the bilinearly interpolated edge (see
* @PSEUDO_GATHER4), and adds 0, 1 or 2, depending on which edges and
* crossing edges are active.
*/
float SMAASearchLength(SMAATexture2D(searchTex), float2 e, float offset) 
{
	// The texture is flipped vertically, with left and right cases taking half
	// of the space horizontally:
	float2 scale = SMAA_SEARCHTEX_SIZE * float2(0.5, -1.0);
		float2 bias = SMAA_SEARCHTEX_SIZE * float2(offset, 1.0);

		// Scale and bias to access texel centers:
		scale += float2(-1.0, 1.0);
	bias += float2(0.5, -0.5);

	// Convert from pixel coordinates to texcoords:
	// (We use SMAA_SEARCHTEX_PACKED_SIZE because the texture is cropped)
	scale *= 1.0 / SMAA_SEARCHTEX_PACKED_SIZE;
	bias *= 1.0 / SMAA_SEARCHTEX_PACKED_SIZE;

	// Lookup the search texture:
	return SMAA_SEARCHTEX_SELECT(SMAASampleLevelZero(searchTex, mad(scale, e, bias)));
}

/**
* Horizontal/vertical search functions for the 2nd pass.
*/
float SMAASearchXLeft(SMAATexture2D(edgesTex), SMAATexture2D(searchTex), float2 texcoord, float end, float4 SMAA_RT_METRICS) 
{
	/**
	* @PSEUDO_GATHER4
	* This texcoord has been offset by (-0.25, -0.125) in the vertex shader to
	* sample between edge, thus fetching four edges in a row.
	* Sampling with different offsets in each direction allows to disambiguate
	* which edges are active from the four fetched ones.
	*/
	float2 e = float2(0.0, 1.0);
	while (texcoord.x > end &&
		e.g > 0.8281 && // Is there some edge not activated?
		e.r == 0.0) { // Or is there a crossing edge that breaks the line?
		e = SMAASampleLevelZero(edgesTex, texcoord).rg;
		texcoord = mad(-float2(2.0, 0.0), SMAA_RT_METRICS.xy, texcoord);
	}

	float offset = mad(-(255.0 / 127.0), SMAASearchLength(SMAATexturePass2D(searchTex), e, 0.0), 3.25);
	return mad(SMAA_RT_METRICS.x, offset, texcoord.x);

	// Non-optimized version:
	// We correct the previous (-0.25, -0.125) offset we applied:
	// texcoord.x += 0.25 * SMAA_RT_METRICS.x;

	// The searches are bias by 1, so adjust the coords accordingly:
	// texcoord.x += SMAA_RT_METRICS.x;

	// Disambiguate the length added by the last step:
	// texcoord.x += 2.0 * SMAA_RT_METRICS.x; // Undo last step
	// texcoord.x -= SMAA_RT_METRICS.x * (255.0 / 127.0) * SMAASearchLength(SMAATexturePass2D(searchTex), e, 0.0);
	// return mad(SMAA_RT_METRICS.x, offset, texcoord.x);
}

float SMAASearchXRight(SMAATexture2D(edgesTex), SMAATexture2D(searchTex), float2 texcoord, float end, float4 SMAA_RT_METRICS) 
{
	float2 e = float2(0.0, 1.0);
	while (texcoord.x < end &&
		e.g > 0.8281 && // Is there some edge not activated?
		e.r == 0.0) { // Or is there a crossing edge that breaks the line?
		e = SMAASampleLevelZero(edgesTex, texcoord).rg;
		texcoord = mad(float2(2.0, 0.0), SMAA_RT_METRICS.xy, texcoord);
	}
	float offset = mad(-(255.0 / 127.0), SMAASearchLength(SMAATexturePass2D(searchTex), e, 0.5), 3.25);
	return mad(-SMAA_RT_METRICS.x, offset, texcoord.x);
}

float SMAASearchYUp(SMAATexture2D(edgesTex), SMAATexture2D(searchTex), float2 texcoord, float end, float4 SMAA_RT_METRICS) 
{
	float2 e = float2(1.0, 0.0);
	while (texcoord.y > end &&
		e.r > 0.8281 && // Is there some edge not activated?
		e.g == 0.0) { // Or is there a crossing edge that breaks the line?
		e = SMAASampleLevelZero(edgesTex, texcoord).rg;
		texcoord = mad(-float2(0.0, 2.0), SMAA_RT_METRICS.xy, texcoord);
	}
	float offset = mad(-(255.0 / 127.0), SMAASearchLength(SMAATexturePass2D(searchTex), e.gr, 0.0), 3.25);
	return mad(SMAA_RT_METRICS.y, offset, texcoord.y);
}

float SMAASearchYDown(SMAATexture2D(edgesTex), SMAATexture2D(searchTex), float2 texcoord, float end, float4 SMAA_RT_METRICS) 
{
	float2 e = float2(1.0, 0.0);
	while (texcoord.y < end &&
		e.r > 0.8281 && // Is there some edge not activated?
		e.g == 0.0) { // Or is there a crossing edge that breaks the line?
		e = SMAASampleLevelZero(edgesTex, texcoord).rg;
		texcoord = mad(float2(0.0, 2.0), SMAA_RT_METRICS.xy, texcoord);
	}
	float offset = mad(-(255.0 / 127.0), SMAASearchLength(SMAATexturePass2D(searchTex), e.gr, 0.5), 3.25);
	return mad(-SMAA_RT_METRICS.y, offset, texcoord.y);
}

/**
* Ok, we have the distance and both crossing edges. So, what are the areas
* at each side of current edge?
*/
float2 SMAAArea(SMAATexture2D(areaTex), float2 dist, float e1, float e2, float offset) 
{
	// Rounding prevents precision errors of bilinear filtering:
	float2 texcoord = mad(float2(SMAA_AREATEX_MAX_DISTANCE, SMAA_AREATEX_MAX_DISTANCE), round(4.0 * float2(e1, e2)), dist);

		// We do a scale and bias for mapping to texel space:
		texcoord = mad(SMAA_AREATEX_PIXEL_SIZE, texcoord, 0.5 * SMAA_AREATEX_PIXEL_SIZE);

	// Move to proper place, according to the subpixel offset:
	texcoord.y = mad(SMAA_AREATEX_SUBTEX_SIZE, offset, texcoord.y);

	// Do it!
	return SMAA_AREATEX_SELECT(SMAASampleLevelZero(areaTex, texcoord));
}

//-----------------------------------------------------------------------------
// Corner Detection Functions
//-----------------------------------------------------------------------------

void SMAADetectHorizontalCornerPattern(SMAATexture2D(edgesTex), inout float2 weights, float4 texcoord, float2 d) 
{
#if !defined(SMAA_DISABLE_CORNER_DETECTION)
	float2 leftRight = step(d.xy, d.yx);
		float2 rounding = (1.0 - SMAA_CORNER_ROUNDING_NORM) * leftRight;

		rounding /= leftRight.x + leftRight.y; // Reduce blending for pixels in the center of a line.

	float2 factor = float2(1.0, 1.0);
		factor.x -= rounding.x * SMAASampleLevelZeroOffset(edgesTex, texcoord.xy, int2(0, 1)).r;
	factor.x -= rounding.y * SMAASampleLevelZeroOffset(edgesTex, texcoord.zw, int2(1, 1)).r;
	factor.y -= rounding.x * SMAASampleLevelZeroOffset(edgesTex, texcoord.xy, int2(0, -2)).r;
	factor.y -= rounding.y * SMAASampleLevelZeroOffset(edgesTex, texcoord.zw, int2(1, -2)).r;

	weights *= saturate(factor);
#endif
}

void SMAADetectVerticalCornerPattern(SMAATexture2D(edgesTex), inout float2 weights, float4 texcoord, float2 d) 
{
#if !defined(SMAA_DISABLE_CORNER_DETECTION)
	float2 leftRight = step(d.xy, d.yx);
		float2 rounding = (1.0 - SMAA_CORNER_ROUNDING_NORM) * leftRight;

		rounding /= leftRight.x + leftRight.y;

	float2 factor = float2(1.0, 1.0);
		factor.x -= rounding.x * SMAASampleLevelZeroOffset(edgesTex, texcoord.xy, int2(1, 0)).g;
	factor.x -= rounding.y * SMAASampleLevelZeroOffset(edgesTex, texcoord.zw, int2(1, 1)).g;
	factor.y -= rounding.x * SMAASampleLevelZeroOffset(edgesTex, texcoord.xy, int2(-2, 0)).g;
	factor.y -= rounding.y * SMAASampleLevelZeroOffset(edgesTex, texcoord.zw, int2(-2, 1)).g;

	weights *= saturate(factor);
#endif
}