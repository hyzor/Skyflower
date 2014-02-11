//-----------------------------------------------------------------------------
// Misc functions

#define SMAAGather(tex, coord) tex.Gather(LinearSampler, coord, 0)

/**
* Gathers current pixel, and the top-left neighbors.
*/
float3 SMAAGatherNeighbours(float2 texcoord,
	float4 offset[3],
	Texture2D tex)
{
	return SMAAGather(tex, texcoord + SMAA_PIXEL_SIZE * float2(-0.5, -0.5)).grb;
}

/**
* Adjusts the threshold by means of predication.
*/
float2 SMAACalculatePredicatedThreshold(float2 texcoord,
	float4 offset[3],
	Texture2D colorTex,
	Texture2D predicationTex) 
{
	float3 neighbours = SMAAGatherNeighbours(texcoord, offset, predicationTex);
	float2 delta = abs(neighbours.xx - neighbours.yz);
	float2 edges = step(SMAA_PREDICATION_THRESHOLD, delta);
	return SMAA_PREDICATION_SCALE * SMAA_THRESHOLD * (1.0 - SMAA_PREDICATION_STRENGTH * edges);
}