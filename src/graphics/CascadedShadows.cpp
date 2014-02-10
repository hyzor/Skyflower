#include "CascadedShadows.h"

CascadedShadows::CascadedShadows(ID3D11Device* device, UINT width, UINT height, UINT nrOfCascades)
{
	this->mNrOfCascades = 0;

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

	Cascade* c = new Cascade(device, width, height);
	this->mCascades.push_back(c);
	this->mNrOfCascades++;

	return true;
}

void CascadedShadows::SetSplitMethod(FrustumSplitMethod method)
{
	this->mFrustumSplitMethod = method;
}

void CascadedShadows::CalcSMSplitDepths(const Camera cam)
{
	float camNear = cam.GetNearZ();
	float camFar = cam.GetFarZ();
	float dist = camFar - camNear;

	//Hardcode for now, every splitdistance is as far the others
	float split = (1.0f / this->mNrOfCascades); //0.33
	float curSplit = split;

	this->mCascades.at(0)->SetSplitDepthNear(camNear);
	for (UINT i = 0; i < this->mNrOfCascades; i++)
	{



		curSplit += split;
	}

	this->mCascades.at(this->mNrOfCascades - 1)->SetSplitDepthFar(camFar);
}

void CascadedShadows::CreateLightFrustums(const DirectionalLight& light, const DirectX::BoundingSphere& sceneBounds, const Camera* cam)
{
	XMMATRIX P;
	XMFLOAT3 frustumCenterLS;

	XMVECTOR lightDir = XMLoadFloat3(&light.Direction);
	XMVECTOR lightPos = -2.0f*sceneBounds.Radius*lightDir;
	XMVECTOR targetPos = XMLoadFloat3(&sceneBounds.Center);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(lightPos, targetPos, up);

	float camNear = cam->GetNearZ(), camFar = cam->GetFarZ();

	if (this->mFrustumSplitMethod == FIT_TO_CASCADE)
	{
		float split = (1.0f / this->mNrOfCascades); // For now, split each frustum equally
		float zNear, zFar;
		zNear = 0.0f;
		zFar = split;

		for (UINT i = 0; i < this->mCascades.size(); i++)
		{
			XMStoreFloat3(&frustumCenterLS, XMVector3TransformCoord(targetPos, V));


			float left = frustumCenterLS.x - sceneBounds.Radius;
			float bottom = frustumCenterLS.y - sceneBounds.Radius;
			float right = frustumCenterLS.x + sceneBounds.Radius;
			float top = frustumCenterLS.y + sceneBounds.Radius;

			P = XMMatrixOrthographicOffCenterLH(left, right, bottom, top, zNear*camNear, zFar*camFar);

			this->mCascades.at(i)->BuildShadowTransform(light, V, P);
			zNear += split;
			zFar += split;
		}
	}
}
