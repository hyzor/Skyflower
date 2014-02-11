#include "CascadedShadows.h"

CascadedShadows::CascadedShadows(ID3D11Device* device, UINT width, UINT height, UINT nrOfCascades)
{
	this->mNrOfCascades = 0;

	this->mFrustumSplitMethod = FIT_TO_CASCADE;
	this->mNearFarFitMethod = FIT_NEARFAR_AABB;

	if (nrOfCascades > CASCADES_MAX || nrOfCascades < 0)
	{
		nrOfCascades = 1;
	}

	for (UINT i = 0; i < nrOfCascades; i++)
	{
		/*Cascade* c = new Cascade(device, width, height);
		this->mCascades.push_back(c);*/
		this->mCascades.push_back(new Cascade(device, width, height));
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

void CascadedShadows::SetSplitMethod(FRUSTUM_SPLIT_METHOD method)
{
	this->mFrustumSplitMethod = method;
}

void CascadedShadows::SetNearFarFitMethod(NEAR_FAR_FIT_METHOD method)
{
	this->mNearFarFitMethod = method;
}

void CascadedShadows::CreateLightFrustums(const DirectionalLight& light, const BoundingSphere& sceneBounds, const BoundingBox& sceneBB, const Camera* cam)
{
	XMMATRIX P;
	XMVECTOR lightOrtographicMin = gVecFLTMAX, lightOrtographicMax = gVecFLTMIN; //Values used as input to create light space frustum
	float intervalBegin, intervalEnd, split;

	XMVECTOR lightDir = XMLoadFloat3(&light.Direction);
	XMVECTOR lightPos = -2.0f*sceneBounds.Radius*lightDir;
	XMVECTOR targetPos = XMLoadFloat3(&sceneBounds.Center);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	//All the cascades will use the same lightview matrix
	XMMATRIX lightView = XMMatrixLookAtLH(lightPos, targetPos, up);

	float camNear = cam->GetNearZ(), camFar = cam->GetFarZ();
	float camRange = camFar - camNear;


	XMFLOAT3 temp[8];
	sceneBB.GetCorners( temp );

	XMVECTOR sceneBBPointsLightSpace[8];
	for (int i = 0; i < 8; i++)
	{
		XMVECTOR v = XMLoadFloat3( &temp[i] );
		sceneBBPointsLightSpace[i] = XMVector3Transform(v, lightView);
	}


	split = (1.0f / this->mNrOfCascades); // For now, split each frustum equally

	intervalBegin = 0.0f;
	intervalEnd = split * camRange;

	for (UINT i = 0; i < this->mCascades.size(); i++)
	{
		if (this->mFrustumSplitMethod == FIT_TO_CASCADE)
		{
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
				//frustumPoints[cIndex] = XMVector4Transform(frustumPoints[cIndex], XMMatrixInverse(nullptr, cam->GetViewMatrix()));

				//Transform to lightspace
				tempTranslatedPoint = XMVector4Transform(frustumPoints[cIndex], lightView);

				//Find closest point
				lightOrtographicMin = XMVectorMin(tempTranslatedPoint, lightOrtographicMin);
				lightOrtographicMax = XMVectorMax(tempTranslatedPoint, lightOrtographicMax);
			}

			if (this->mFrustumSplitMethod == FIT_TO_CASCADE)
			{
				//TODO: optimize specifically for the FIT_TO_CASCADE method
			}
			else if (this->mFrustumSplitMethod == FIT_TO_SCENE)
			{
				//TODO: optimize specifically for the FIT_TO_SCENE method (shimmering effects)
			}

			float lightOrtographicMinZ = XMVectorGetZ(lightOrtographicMin);

			if (false)
			{
				//TODO: Prevent shadowjittering??
			}

			float nearPlane = 0.0f;
			float farPlane = 10000.0f;

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

			}

			//nearPlane = intervalBegin;
			//farPlane = intervalEnd;

			//The resulting projection matrix for the current cascade
			P = XMMatrixOrthographicOffCenterLH(
				XMVectorGetX(lightOrtographicMin), XMVectorGetX(lightOrtographicMax), 
				XMVectorGetY(lightOrtographicMin), XMVectorGetY(lightOrtographicMax), 
				nearPlane, farPlane);

			this->mCascades.at(i)->SetShadowMatrices(lightView, P, lightPos);

			//Increment beginning and end equally to create the next subfrustum
			intervalBegin += split * camRange;
			intervalEnd += split * camRange;
		}
	}
		
}


void CascadedShadows::RenderSceneToCascades(
	const std::vector<ModelInstanceImpl*>& modelInstances,
	const std::vector<AnimatedInstanceImpl*>& mAnimatedInstances,
	const std::vector<MorphModelInstance*>& mMorphInstances,
	ID3D11DeviceContext* deviceContext,
	ShadowShader* shadowShader,
	SkinnedShadowShader* skinnedShadowShader,
	ShadowMorphShader* shadowMorphShader)
{
	for (UINT i = 0; i < this->mCascades.size(); i++)
	{
		this->mCascades.at(i)->BindDsvAndSetNullRenderTarget(deviceContext);
		this->mCascades.at(i)->DrawSceneToShadowMap(
			modelInstances, 
			mAnimatedInstances, 
			mMorphInstances, 
			deviceContext, 
			shadowShader, 
			skinnedShadowShader, 
			shadowMorphShader);
	}
}

Cascade* CascadedShadows::GetCascade(UINT index)
{
	if (index < this->mNrOfCascades && index >= 0)
		return this->mCascades.at(index);
	else 
		return nullptr;
}
