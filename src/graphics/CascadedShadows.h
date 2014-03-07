#ifndef GRAPHICS_CASCADEDSHADOWS_H
#define GRAPHICS_CASCADEDSHADOWS_H

#include <vector>
#include "Cascade.h"
#include "Camera.h"
#include "RenderStates.h"

#define CASCADES_MAX 4
#define CASCADESPLITS CASCADES_MAX - 1

static const XMVECTORF32 gVecFLTMAX = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };
static const XMVECTORF32 gVecFLTMIN = { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
static const XMVECTORF32 gVecZero = { 0.0f, 0.0f, 0.0f, 0.0f };

enum FRUSTUM_SPLIT_METHOD
{
	FIT_TO_SCENE, //The cascades overlap eachother (e.g: 0 - 0.33, 0-0.66, 0-1.0)
	FIT_TO_CASCADE //The cascades do not overlap eachother (e.g: 0 - 0.33, 0.33-0.66, 0.66-1.0)
};

enum NEAR_FAR_FIT_METHOD
{
	FIT_NEARFAR_PANCAKING,
	FIT_NEARFAR_ZERO_ONE,
	FIT_NEARFAR_AABB,
	FIT_NEARFAR_SCENE_AABB
};

/*
	Used when computing near and far values for cascades
*/
struct HelpTriangle
{
	XMVECTOR pt[3];
	bool culled;
};

class CascadedShadows
{

private:
	std::vector<Cascade*> mCascades;
	UINT mNrOfCascades;

	FRUSTUM_SPLIT_METHOD mFrustumSplitMethod;
	NEAR_FAR_FIT_METHOD mNearFarFitMethod;

	//Splitdepths used by cascades, stored in a 0-1 interval (0 to 100%)
	float mCascadeSplits[CASCADESPLITS];

	float mBufferSize;

public:
	//Adds 1 cascade automatically if no argument is given
	CascadedShadows(ID3D11Device* device, UINT width, UINT height, UINT nrOfCascades = 1);

	~CascadedShadows(void);

	//Returns false if the maximum amount of cascades has been reached
	bool AddCascade(ID3D11Device* device, UINT width, UINT height);

	//Set splitdepth for split of the given index, returns false if index is bad
	bool SetSplitDepth(float depth, UINT cascadeIndex);

	//Set the method which will be used to create the split up lightfrustums
	void SetSplitMethod(FRUSTUM_SPLIT_METHOD splitMethod);

	//Set the method to calculate the near and far planes to be used for the separate lightfrustums
	void SetNearFarFitMethod(NEAR_FAR_FIT_METHOD nearFarMethod);

	//Create the different light frustums using currently selected methods
	void CreateLightFrustums(const DLight& light, const BoundingSphere& sceneBounds, const BoundingBox& sceneBB, const Camera* cam);

	void RenderSceneToCascades(
		const std::vector<ModelInstanceImpl*>& modelInstances,
		const std::vector<AnimatedInstanceImpl*>& mAnimatedInstances,
		const std::vector<MorphModelInstanceImpl*>& mMorphInstances,
		const std::vector<SortedAnimatedInstanceImpl*>& mSkinnedSortedInstances,
		ID3D11DeviceContext* deviceContext,
		ShadowShader* shadowShader,
		SkinnedShadowShader* skinnedShadowShader,
		ShadowMorphShader* shadowMorphShader,
		BasicDeferredSkinnedSortedShadowShader* skinnedSortedShadowShader);

	Cascade* GetCascade(UINT index);

	UINT GetNrOfCascades() const;

private:
	void ComputeNearFar(float& _near, float& _far, XMVECTOR lightOrtographicMin, XMVECTOR lightOrtographicMax, XMVECTOR* sceneAABBPointsLightSpace);

};

#endif