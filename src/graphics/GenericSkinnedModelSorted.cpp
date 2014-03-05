#include "GenericSkinnedModelSorted.h"

// Must be included last!
#include "shared/debug.h"

GenericSkinnedModelSorted::GenericSkinnedModelSorted(ID3D11Device* device, TextureManager* textureMgr, const std::string& fileName)
{
	std::vector<GenericMaterial> mats;

	// Load all the meshes
	GenericObjectLoader objectLoader;
	objectLoader.LoadSkinnedObjectSorted(fileName, mats, meshes, skinnedData);

	numMeshes = (UINT)meshes.size();
	numMaterials = (UINT)mats.size();

	// Set vertices and indices for each mesh
	for (UINT i = 0; i < numMeshes; ++i)
	{
		meshes[i].setIndices(device, &meshes[i].mIndices[0], (UINT)meshes[i].mIndices.size());
		meshes[i].setVertices(device, &meshes[i].mVertices[0], (UINT)meshes[i].mVertices.size());
	}

	for (UINT i = 0; i < numMaterials; ++i)
	{
		mat.push_back(mats[i].mat);

		if (mats[i].diffuseMapName != "")
		{
			/*
			ID3D11ShaderResourceView* _diffuseMapSRV = textureMgr->CreateLevelTexture(mats[i].diffuseMapName);
			diffuseMapSRV.push_back(_diffuseMapSRV);
			*/
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _diffuseMapSRV = nullptr;
			_diffuseMapSRV = textureMgr->CreateLevelTexture(mats[i].diffuseMapName);
			diffuseMapSRV.push_back(_diffuseMapSRV);
		}
		else
		{
			diffuseMapSRV.push_back(NULL);
		}

		if (mats[i].normalMapName != "")
		{
			/*
			ID3D11ShaderResourceView* _normalMapSRV = textureMgr->CreateLevelTexture(mats[i].normalMapName);
			normalMapSRV.push_back(_normalMapSRV);
			*/
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _normalMapSRV = nullptr;
			_normalMapSRV = textureMgr->CreateLevelTexture(mats[i].normalMapName);
			normalMapSRV.push_back(_normalMapSRV);
		}
		else
		{
			normalMapSRV.push_back(NULL);
		}
	}

	//--------------------------------------------------------
	// Compute bounding box
	//--------------------------------------------------------
	// Compute vertex position extreme values
	XMFLOAT3 vMinf3(+MathHelper::infinity, +MathHelper::infinity, +MathHelper::infinity);
	XMFLOAT3 vMaxf3(-MathHelper::infinity, -MathHelper::infinity, -MathHelper::infinity);

	XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

	for (UINT i = 0; i < numMeshes; ++i)
	{
		for (UINT j = 0; j < meshes[i].mVertices.size(); ++j)
		{
			XMVECTOR pos = XMLoadFloat3(&meshes[i].mVertices[j].position);

			vMin = XMVectorMin(vMin, pos);
			vMax = XMVectorMax(vMax, pos);
		}
	}

	// Store values in bounding box
	XMStoreFloat3(&boundingBox.Center, 0.5f*(vMin+vMax));
	XMStoreFloat3(&boundingBox.Extents, 0.5f*(vMax-vMin));

	//ComputeBoundingAxisAlignedBoxFromPoints(&boundingBox, vertices.size(), &vertices[0]->position, sizeof(Vertex::Basic32));
}


GenericSkinnedModelSorted::~GenericSkinnedModelSorted(void)
{
	diffuseMapSRV.clear();
	normalMapSRV.clear();
}

void GenericSkinnedModelSortedInstance::Update(float dt)
{
	//TimePos += dt;
	lowerTimePos += dt * lowerAnimationSpeed;
	upperTimePos += dt * upperAnimationSpeed;

	if (!loopUpperbodyAnim)
	{
		float animationDuration = model->skinnedData.GetAnimationDuration(AnimationIndex, upperBodyFrameStart, upperBodyFrameEnd);

		if (upperTimePos > animationDuration)
		{
			upperTimePos = animationDuration;
			upperAnimationDone = true;
		}
	}

	if (!loopLowerBodyAnim)
	{
		float animationDuration = model->skinnedData.GetAnimationDuration(AnimationIndex, lowerBodyFrameStart, lowerBodyFrameEnd);

		if (lowerTimePos > animationDuration)
		{
			lowerTimePos = animationDuration;
			lowerAnimationDone = true;
		}
	}

	//if (!loop)
		//TimePos = min(TimePos, model->skinnedData.GetAnimationDuration(AnimationIndex, frameStart, frameEnd));

	//FinalTransforms = model->skinnedData.GetTransforms(TimePos, AnimationIndex, frameStart, frameEnd, playAnimForward);

	FinalLowerBodyTransforms = model->skinnedData.GetLowerBodyTransforms(lowerTimePos, AnimationIndex, lowerBodyFrameStart, lowerBodyFrameEnd, playLowerBodyAnimForward);
	FinalUpperBodyTransforms = model->skinnedData.GetUpperBodyTransforms(upperTimePos, AnimationIndex, upperBodyFrameStart, upperBodyFrameEnd, playUpperBodyAnimForward);
}

GenericSkinnedModelSortedInstance::GenericSkinnedModelSortedInstance()
{
	model = nullptr;
	isVisible = false;
	AnimationName = "";
	AnimationIndex = 0;
	//TimePos = 0.0f;

	upperBodyFrameStart = 0;
	upperBodyFrameEnd = 1;
	playUpperBodyAnimForward = true;
	loopUpperbodyAnim = true;
	upperAnimationDone = false;
	upperTimePos = 0.0f;

	lowerBodyFrameStart = 0;
	lowerBodyFrameEnd = 1;
	playLowerBodyAnimForward = true;
	loopLowerBodyAnim = true;
	upperAnimationDone = false;
	lowerTimePos = 0.0f;
}

GenericSkinnedModelSortedInstance::~GenericSkinnedModelSortedInstance()
{

}
