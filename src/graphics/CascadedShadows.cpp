#include "CascadedShadows.h"

CascadedShadows::CascadedShadows(ID3D11Device* device, UINT width, UINT height, UINT nrOfCascades)
{
	this->mNrOfCascades = 0;
	this->mBufferSize = (float)width;
	this->mFrustumSplitMethod = FIT_TO_CASCADE;
	this->mNearFarFitMethod = FIT_NEARFAR_AABB;

	if (nrOfCascades > CASCADES_MAX || nrOfCascades < 0)
	{
		nrOfCascades = 1;
	}

	for (UINT i = 0; i < nrOfCascades; i++)
	{
		Cascade* c = new Cascade(device, width, height);
		this->mCascades.push_back(c);
		this->mNrOfCascades++;
	}
}

CascadedShadows::~CascadedShadows()
{
	for (UINT i = 0; i < this->mCascades.size(); i++)
	{
		delete this->mCascades.at(i);
	}
	this->mCascades.clear();
}

bool CascadedShadows::AddCascade(ID3D11Device* device, UINT width, UINT height)
{
	bool result = false;

	if (this->mNrOfCascades < CASCADES_MAX)
	{
		Cascade* c = new Cascade(device, width, height);
		this->mCascades.push_back(c);
		this->mNrOfCascades++;

		result = true;
	}

	return result;
}

bool CascadedShadows::SetSplitDepth(float depth, UINT cascadeIndex)
{
	bool result = false;

	//Keep 1 split less than the number of cascades (3 cascades - 2 splits, 2 cascades - 1 split, etc)
	if (cascadeIndex >= 0 && cascadeIndex < this->mNrOfCascades - 1)
	{
		this->mCascadeSplits[cascadeIndex] = depth;
		result = true;
	}

	return result;
}

void CascadedShadows::SetSplitMethod(FRUSTUM_SPLIT_METHOD method)
{
	this->mFrustumSplitMethod = method;
}

void CascadedShadows::SetNearFarFitMethod(NEAR_FAR_FIT_METHOD method)
{
	this->mNearFarFitMethod = method;
}

void CascadedShadows::CreateLightFrustums(const DLight& light, const BoundingSphere& sceneBounds, const BoundingBox& sceneBB, const Camera* cam)
{
	XMMATRIX P;
	XMVECTOR lightOrtographicMin = gVecFLTMAX, lightOrtographicMax = gVecFLTMIN; //Values used as input to create light space frustum
	float intervalBegin, intervalEnd;

	XMVECTOR lightDir = XMLoadFloat3(&light.Direction);
	XMVECTOR lightPos = -2.0f*(sceneBounds.Radius*0.5f)*lightDir;
	//XMVECTOR targetPos = XMLoadFloat3(&sceneBounds.Center);
	XMVECTOR targetPos = { 0.0f, 0.0f, 0.0f, 0.0f };// XMLoadFloat3(&sceneBB.Center);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	//All cascades will use the same lightview matrix
	XMMATRIX lightView = XMMatrixLookAtLH(lightPos, targetPos, up);

	float camNear = cam->GetNearZ(), camFar = cam->GetFarZ();
	float camRange = camFar - camNear;
	if (camRange > 1000)
		camRange = 1000;


	XMFLOAT3 temp[8];
	sceneBB.GetCorners( temp );

	XMVECTOR sceneBBPointsLightSpace[8];
	for (int i = 0; i < 8; i++)
	{
		XMVECTOR v = XMLoadFloat3( &temp[i] );
		sceneBBPointsLightSpace[i] = XMVector3Transform(v, lightView);
	}

	XMVECTOR worldUnitsPerTexel = gVecZero;

	for (UINT i = 0; i < this->mCascades.size(); i++)
	{
		if (this->mFrustumSplitMethod == FIT_TO_CASCADE)
		{

			if (i == 0)
			{
				intervalBegin = 0.0f;
				intervalEnd = this->mCascadeSplits[i] * camRange; //Multiply with range of camera since splits are stored in 0-1 interval
			}
			else if (i > 0 && i < this->mNrOfCascades - 1)
			{
				intervalBegin = this->mCascades.at(i - 1)->GetSplitDepthFar();
				intervalEnd = this->mCascadeSplits[i] * camRange;
			}
			else
			{
				intervalBegin = this->mCascades.at(i - 1)->GetSplitDepthFar();
				intervalEnd = camRange;
			}

			//Create frustum points for this cascade interval
			XMVECTOR frustumPoints[8];

 			BoundingFrustum camFrustum(cam->GetProjMatrix());
			camFrustum.Near = intervalBegin;
			camFrustum.Far = intervalEnd;

			static const XMVECTORU32 grabY = { 0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000 };
			static const XMVECTORU32 grabX = { 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000 };

			XMVECTORF32 rightTop = { camFrustum.RightSlope, camFrustum.TopSlope, 1.0f, 1.0f };
			XMVECTORF32 leftBottom = { camFrustum.LeftSlope, camFrustum.BottomSlope, 1.0f, 1.0f };
			XMVECTORF32 _near = { camFrustum.Near, camFrustum.Near, camFrustum.Near, 1.0f };
			XMVECTORF32 _far = { camFrustum.Far, camFrustum.Far, camFrustum.Far, 1.0f };
			XMVECTOR rightTopNear = XMVectorMultiply(rightTop, _near);
			XMVECTOR rightTopFar = XMVectorMultiply(rightTop, _far);
			XMVECTOR leftBottomNear = XMVectorMultiply(leftBottom, _near);
			XMVECTOR leftBottomFar = XMVectorMultiply(leftBottom, _far);

			frustumPoints[0] = rightTopNear;
			frustumPoints[1] = XMVectorSelect(rightTopNear, leftBottomNear, grabX);
			frustumPoints[2] = leftBottomNear;
			frustumPoints[3] = XMVectorSelect(rightTopNear, leftBottomNear, grabY);
			frustumPoints[4] = rightTopFar;
			frustumPoints[5] = XMVectorSelect(rightTopFar, leftBottomFar, grabX);
			frustumPoints[6] = leftBottomFar;
			frustumPoints[7] = XMVectorSelect(rightTopFar, leftBottomFar, grabY);


			XMVECTOR tempTranslatedPoint;

			for (int cIndex = 0; cIndex < 8; cIndex++)
			{
				//Transform from camera view space to world
				frustumPoints[cIndex] = XMVector4Transform(frustumPoints[cIndex], XMMatrixInverse(nullptr, cam->GetViewMatrix()));

				//Transform to lightspace
				tempTranslatedPoint = XMVector4Transform(frustumPoints[cIndex], lightView);

				//Find closest point
				lightOrtographicMin = XMVectorMin(tempTranslatedPoint, lightOrtographicMin);
				lightOrtographicMax = XMVectorMax(tempTranslatedPoint, lightOrtographicMax);
			}

			if (this->mFrustumSplitMethod == FIT_TO_CASCADE)
			{
				float fNormalizeByBufferSize = (1.0f / this->mBufferSize);
				XMVECTOR vNormalizeByBufferSize = XMVectorSet(fNormalizeByBufferSize, fNormalizeByBufferSize, 0.0f, 0.0f);

				XMVECTOR borderOffset = lightOrtographicMax - lightOrtographicMin;
				XMVECTORF32 halfVec = { 0.5f, 0.5f, 0.5f, 0.5f };

				borderOffset *= halfVec;
				lightOrtographicMax += borderOffset;
				lightOrtographicMin -= borderOffset;

				worldUnitsPerTexel = lightOrtographicMax - lightOrtographicMin;
				worldUnitsPerTexel *= vNormalizeByBufferSize;
			}
			else if (this->mFrustumSplitMethod == FIT_TO_SCENE)
			{
				//TODO: optimize specifically for the FIT_TO_SCENE method (shimmering effects)
			}

			//Prevent shimmering edges when moving the camera (also for zooming, but not rotating)
			if (true)
			{
				lightOrtographicMin /= worldUnitsPerTexel;
				lightOrtographicMin = XMVectorFloor(lightOrtographicMin);
				lightOrtographicMin *= worldUnitsPerTexel;

				lightOrtographicMax /= worldUnitsPerTexel;
				lightOrtographicMax = XMVectorFloor(lightOrtographicMax);
				lightOrtographicMax *= worldUnitsPerTexel;
			}

			float nearPlane = FLT_MAX;
			float farPlane = -FLT_MAX;

			if (this->mNearFarFitMethod == FIT_NEARFAR_AABB)
			{
				XMVECTOR lightSpaceSceneAABBMin = gVecFLTMAX;
				XMVECTOR lightSpaceSceneAABBMax = gVecFLTMIN;

				for (int index = 0; index < 8; index++)
				{
					lightSpaceSceneAABBMin = XMVectorMin(sceneBBPointsLightSpace[index], lightSpaceSceneAABBMin);
					lightSpaceSceneAABBMax = XMVectorMax(sceneBBPointsLightSpace[index], lightSpaceSceneAABBMax);
				}

				nearPlane = XMVectorGetZ(lightSpaceSceneAABBMin);
				farPlane = XMVectorGetZ(lightSpaceSceneAABBMax);
			}
			else if (this->mNearFarFitMethod == FIT_NEARFAR_SCENE_AABB
				|| this->mNearFarFitMethod == FIT_NEARFAR_PANCAKING)
			{
				ComputeNearFar(nearPlane, farPlane, lightOrtographicMin, lightOrtographicMax, sceneBBPointsLightSpace);
				if (XMVectorGetZ(lightOrtographicMin) > nearPlane)
				{
					nearPlane = XMVectorGetZ(lightOrtographicMin);
				}
			}

			P = XMMatrixOrthographicOffCenterLH(
				XMVectorGetX(lightOrtographicMin), XMVectorGetX(lightOrtographicMax),
				XMVectorGetY(lightOrtographicMin), XMVectorGetY(lightOrtographicMax),
				nearPlane, farPlane);

			this->mCascades.at(i)->SetShadowMatrices(lightView, P, lightPos);

			//Set the depths of the intervals to later use these to determine correct cascade to sample from
			this->mCascades.at(i)->SetSplitDepthNear(intervalBegin);
			this->mCascades.at(i)->SetSplitDepthFar(intervalEnd);;
		}
	}
		
}


void CascadedShadows::RenderSceneToCascades(
	const std::vector<ModelInstanceImpl*>& modelInstances,
	const std::vector<AnimatedInstanceImpl*>& mAnimatedInstances,
	const std::vector<MorphModelInstanceImpl*>& mMorphInstances,
	const std::vector<SortedAnimatedInstanceImpl*>& mSkinnedSortedInstances,
	ID3D11DeviceContext* deviceContext,
	ShadowShader* shadowShader,
	SkinnedShadowShader* skinnedShadowShader,
	ShadowMorphShader* shadowMorphShader,
	BasicDeferredSkinnedSortedShadowShader* skinnedSortedShadowShader
	)
{
	for (UINT i = 0; i < this->mCascades.size(); i++)
	{
		this->mCascades.at(i)->BindDsvAndSetNullRenderTarget(deviceContext);
		switch (i)
		{
		case 0:
			deviceContext->RSSetState(RenderStates::mDepthBiasCloseToEyeRS);
			break;
		case 1:
			deviceContext->RSSetState(RenderStates::mDepthBiasFarFromEyeRS);
			break;
		case 2:
			deviceContext->RSSetState(RenderStates::mDepthBiasSuperFarFromEyeRS);
			break;
		case 3:
			deviceContext->RSSetState(RenderStates::mDepthBiasSuperFarFromEyeRS);
			break;
		}
		this->mCascades.at(i)->DrawSceneToShadowMap(
			modelInstances, 
			mAnimatedInstances, 
			mMorphInstances,
			mSkinnedSortedInstances,
			deviceContext, 
			shadowShader, 
			skinnedShadowShader, 
			shadowMorphShader,
			skinnedSortedShadowShader);
	}
}

Cascade* CascadedShadows::GetCascade(UINT index)
{
	if (index < this->mNrOfCascades && index >= 0)
		return this->mCascades.at(index);
	else 
		return nullptr;
}

UINT CascadedShadows::GetNrOfCascades() const
{
	return this->mNrOfCascades;
}

void CascadedShadows::ComputeNearFar(FLOAT& fNearPlane,
	FLOAT& fFarPlane,
	FXMVECTOR vLightCameraOrthographicMin,
	FXMVECTOR vLightCameraOrthographicMax,
	XMVECTOR* pvPointsInCameraView)
{
	// Initialize the near and far planes
	fNearPlane = FLT_MAX;
	fFarPlane = -FLT_MAX;

	HelpTriangle triangleList[16];
	INT iTriangleCnt = 1;

	triangleList[0].pt[0] = pvPointsInCameraView[0];
	triangleList[0].pt[1] = pvPointsInCameraView[1];
	triangleList[0].pt[2] = pvPointsInCameraView[2];
	triangleList[0].culled = false;

	// These are the indices used to tesselate an AABB into a list of triangles.
	static const INT iAABBTriIndexes[] =
	{
		0, 1, 2, 1, 2, 3,
		4, 5, 6, 5, 6, 7,
		0, 2, 4, 2, 4, 6,
		1, 3, 5, 3, 5, 7,
		0, 1, 4, 1, 4, 5,
		2, 3, 6, 3, 6, 7
	};

	INT iPointPassesCollision[3];

	// At a high level: 
	// 1. Iterate over all 12 triangles of the AABB.  
	// 2. Clip the triangles against each plane. Create new triangles as needed.
	// 3. Find the min and max z values as the near and far plane.

	//This is easier because the triangles are in camera spacing making the collisions tests simple comparisions.

	float fLightCameraOrthographicMinX = XMVectorGetX(vLightCameraOrthographicMin);
	float fLightCameraOrthographicMaxX = XMVectorGetX(vLightCameraOrthographicMax);
	float fLightCameraOrthographicMinY = XMVectorGetY(vLightCameraOrthographicMin);
	float fLightCameraOrthographicMaxY = XMVectorGetY(vLightCameraOrthographicMax);

	for (INT AABBTriIter = 0; AABBTriIter < 12; ++AABBTriIter)
	{

		triangleList[0].pt[0] = pvPointsInCameraView[iAABBTriIndexes[AABBTriIter * 3 + 0]];
		triangleList[0].pt[1] = pvPointsInCameraView[iAABBTriIndexes[AABBTriIter * 3 + 1]];
		triangleList[0].pt[2] = pvPointsInCameraView[iAABBTriIndexes[AABBTriIter * 3 + 2]];
		iTriangleCnt = 1;
		triangleList[0].culled = FALSE;

		// Clip each invidual triangle against the 4 frustums.  When ever a triangle is clipped into new triangles, 
		//add them to the list.
		for (INT frustumPlaneIter = 0; frustumPlaneIter < 4; ++frustumPlaneIter)
		{

			FLOAT fEdge;
			INT iComponent;

			if (frustumPlaneIter == 0)
			{
				fEdge = fLightCameraOrthographicMinX; // todo make float temp
				iComponent = 0;
			}
			else if (frustumPlaneIter == 1)
			{
				fEdge = fLightCameraOrthographicMaxX;
				iComponent = 0;
			}
			else if (frustumPlaneIter == 2)
			{
				fEdge = fLightCameraOrthographicMinY;
				iComponent = 1;
			}
			else
			{
				fEdge = fLightCameraOrthographicMaxY;
				iComponent = 1;
			}

			for (INT triIter = 0; triIter < iTriangleCnt; ++triIter)
			{
				// We don't delete triangles, so we skip those that have been culled.
				if (!triangleList[triIter].culled)
				{
					INT iInsideVertCount = 0;
					XMVECTOR tempOrder;
					// Test against the correct frustum plane.
					// This could be written more compactly, but it would be harder to understand.

					if (frustumPlaneIter == 0)
					{
						for (INT triPtIter = 0; triPtIter < 3; ++triPtIter)
						{
							if (XMVectorGetX(triangleList[triIter].pt[triPtIter]) >
								XMVectorGetX(vLightCameraOrthographicMin))
							{
								iPointPassesCollision[triPtIter] = 1;
							}
							else
							{
								iPointPassesCollision[triPtIter] = 0;
							}
							iInsideVertCount += iPointPassesCollision[triPtIter];
						}
					}
					else if (frustumPlaneIter == 1)
					{
						for (INT triPtIter = 0; triPtIter < 3; ++triPtIter)
						{
							if (XMVectorGetX(triangleList[triIter].pt[triPtIter]) <
								XMVectorGetX(vLightCameraOrthographicMax))
							{
								iPointPassesCollision[triPtIter] = 1;
							}
							else
							{
								iPointPassesCollision[triPtIter] = 0;
							}
							iInsideVertCount += iPointPassesCollision[triPtIter];
						}
					}
					else if (frustumPlaneIter == 2)
					{
						for (INT triPtIter = 0; triPtIter < 3; ++triPtIter)
						{
							if (XMVectorGetY(triangleList[triIter].pt[triPtIter]) >
								XMVectorGetY(vLightCameraOrthographicMin))
							{
								iPointPassesCollision[triPtIter] = 1;
							}
							else
							{
								iPointPassesCollision[triPtIter] = 0;
							}
							iInsideVertCount += iPointPassesCollision[triPtIter];
						}
					}
					else
					{
						for (INT triPtIter = 0; triPtIter < 3; ++triPtIter)
						{
							if (XMVectorGetY(triangleList[triIter].pt[triPtIter]) <
								XMVectorGetY(vLightCameraOrthographicMax))
							{
								iPointPassesCollision[triPtIter] = 1;
							}
							else
							{
								iPointPassesCollision[triPtIter] = 0;
							}
							iInsideVertCount += iPointPassesCollision[triPtIter];
						}
					}

					// Move the points that pass the frustum test to the begining of the array.
					if (iPointPassesCollision[1] && !iPointPassesCollision[0])
					{
						tempOrder = triangleList[triIter].pt[0];
						triangleList[triIter].pt[0] = triangleList[triIter].pt[1];
						triangleList[triIter].pt[1] = tempOrder;
						iPointPassesCollision[0] = TRUE;
						iPointPassesCollision[1] = FALSE;
					}
					if (iPointPassesCollision[2] && !iPointPassesCollision[1])
					{
						tempOrder = triangleList[triIter].pt[1];
						triangleList[triIter].pt[1] = triangleList[triIter].pt[2];
						triangleList[triIter].pt[2] = tempOrder;
						iPointPassesCollision[1] = TRUE;
						iPointPassesCollision[2] = FALSE;
					}
					if (iPointPassesCollision[1] && !iPointPassesCollision[0])
					{
						tempOrder = triangleList[triIter].pt[0];
						triangleList[triIter].pt[0] = triangleList[triIter].pt[1];
						triangleList[triIter].pt[1] = tempOrder;
						iPointPassesCollision[0] = TRUE;
						iPointPassesCollision[1] = FALSE;
					}

					if (iInsideVertCount == 0)
					{ // All points failed. We're done,  
						triangleList[triIter].culled = true;
					}
					else if (iInsideVertCount == 1)
					{// One point passed. Clip the triangle against the Frustum plane
						triangleList[triIter].culled = false;

						// 
						XMVECTOR vVert0ToVert1 = triangleList[triIter].pt[1] - triangleList[triIter].pt[0];
						XMVECTOR vVert0ToVert2 = triangleList[triIter].pt[2] - triangleList[triIter].pt[0];

						// Find the collision ratio.
						FLOAT fHitPointTimeRatio = fEdge - XMVectorGetByIndex(triangleList[triIter].pt[0], iComponent);
						// Calculate the distance along the vector as ratio of the hit ratio to the component.
						FLOAT fDistanceAlongVector01 = fHitPointTimeRatio / XMVectorGetByIndex(vVert0ToVert1, iComponent);
						FLOAT fDistanceAlongVector02 = fHitPointTimeRatio / XMVectorGetByIndex(vVert0ToVert2, iComponent);
						// Add the point plus a percentage of the vector.
						vVert0ToVert1 *= fDistanceAlongVector01;
						vVert0ToVert1 += triangleList[triIter].pt[0];
						vVert0ToVert2 *= fDistanceAlongVector02;
						vVert0ToVert2 += triangleList[triIter].pt[0];

						triangleList[triIter].pt[1] = vVert0ToVert2;
						triangleList[triIter].pt[2] = vVert0ToVert1;

					}
					else if (iInsideVertCount == 2)
					{ // 2 in  // tesselate into 2 triangles


						// Copy the triangle\(if it exists) after the current triangle out of
						// the way so we can override it with the new triangle we're inserting.
						triangleList[iTriangleCnt] = triangleList[triIter + 1];

						triangleList[triIter].culled = false;
						triangleList[triIter + 1].culled = false;

						// Get the vector from the outside point into the 2 inside points.
						XMVECTOR vVert2ToVert0 = triangleList[triIter].pt[0] - triangleList[triIter].pt[2];
						XMVECTOR vVert2ToVert1 = triangleList[triIter].pt[1] - triangleList[triIter].pt[2];

						// Get the hit point ratio.
						FLOAT fHitPointTime_2_0 = fEdge - XMVectorGetByIndex(triangleList[triIter].pt[2], iComponent);
						FLOAT fDistanceAlongVector_2_0 = fHitPointTime_2_0 / XMVectorGetByIndex(vVert2ToVert0, iComponent);
						// Calcaulte the new vert by adding the percentage of the vector plus point 2.
						vVert2ToVert0 *= fDistanceAlongVector_2_0;
						vVert2ToVert0 += triangleList[triIter].pt[2];

						// Add a new triangle.
						triangleList[triIter + 1].pt[0] = triangleList[triIter].pt[0];
						triangleList[triIter + 1].pt[1] = triangleList[triIter].pt[1];
						triangleList[triIter + 1].pt[2] = vVert2ToVert0;

						//Get the hit point ratio.
						FLOAT fHitPointTime_2_1 = fEdge - XMVectorGetByIndex(triangleList[triIter].pt[2], iComponent);
						FLOAT fDistanceAlongVector_2_1 = fHitPointTime_2_1 / XMVectorGetByIndex(vVert2ToVert1, iComponent);
						vVert2ToVert1 *= fDistanceAlongVector_2_1;
						vVert2ToVert1 += triangleList[triIter].pt[2];
						triangleList[triIter].pt[0] = triangleList[triIter + 1].pt[1];
						triangleList[triIter].pt[1] = triangleList[triIter + 1].pt[2];
						triangleList[triIter].pt[2] = vVert2ToVert1;
						// Cncrement triangle count and skip the triangle we just inserted.
						++iTriangleCnt;
						++triIter;


					}
					else
					{ // all in
						triangleList[triIter].culled = false;

					}
				}// end if !culled loop            
			}
		}
		for (INT index = 0; index < iTriangleCnt; ++index)
		{
			if (!triangleList[index].culled)
			{
				// Set the near and far plan and the min and max z values respectivly.
				for (int vertind = 0; vertind < 3; ++vertind)
				{
					float fTriangleCoordZ = XMVectorGetZ(triangleList[index].pt[vertind]);
					if (fNearPlane > fTriangleCoordZ)
					{
						fNearPlane = fTriangleCoordZ;
					}
					if (fFarPlane  <fTriangleCoordZ)
					{
						fFarPlane = fTriangleCoordZ;
					}
				}
			}
		}
	}
}