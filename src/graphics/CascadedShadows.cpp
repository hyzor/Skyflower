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
}

void CascadedShadows::SetSplitMethod(FrustumSplitMethod method)
{
	this->mFrustumSplitMethod = method;
}