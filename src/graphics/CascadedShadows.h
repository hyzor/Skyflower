#ifndef GRAPHICS_CASCADEDSHADOWS_H
#define GRAPHICS_CASCADEDSHADOWS_H

#include <vector>
#include "Cascade.h"
#include "Camera.h"

#define CASCADES_MAX 3

enum FrustumSplitMethod
{
	FIT_TO_SCENE,
	FIT_TO_CASCADE
};

class CascadedShadows
{

private:
	std::vector<Cascade*> mCascades;
	UINT mNrOfCascades;
	FrustumSplitMethod mFrustumSplitMethod;

public:
	CascadedShadows(ID3D11Device* device, UINT width, UINT height, UINT nrOfCascades);
	~CascadedShadows(void);

	//Returns false if the maximum amount of cascades has been reached
	bool AddCascade(ID3D11Device* device, UINT width, UINT height);

	void SetSplitMethod(FrustumSplitMethod splitMethod);
	void CalcSMSplitDepths(const Camera cam);
	void CreateLightFrustums(const DirectionalLight& light, const DirectX::BoundingSphere& sceneBounds, const Camera* cam);

};

#endif