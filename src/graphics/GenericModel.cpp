#include "GenericModel.h"

// Must be included last!
#include "shared/debug.h"

GenericModel::GenericModel(ID3D11Device* device, TextureManager* textureMgr, const std::string& fileName)
{
	std::vector<GenericMaterial> mats;

	// Load all the meshes
	GenericObjectLoader objectLoader;
	//objectLoader.loadObject(fileName, mats, meshes, device);
	objectLoader.loadObject(fileName, mats, meshes);

	meshCount = (UINT)meshes.size();
	materialCount = (UINT)mats.size();

	// Set vertices and indices for each mesh
	for (UINT i = 0; i < meshCount; ++i)
	{
		meshes[i].SetIndices(device, &meshes[i].indices[0], (UINT)meshes[i].indices.size());
		meshes[i].SetVertices(device, &meshes[i].vertices[0], (UINT)meshes[i].vertices.size());

		//// Push back addresses to all the vertices to the models vector of pointers
		//for (UINT j = 0; j < meshes[i].vertices.size(); ++j)
		//	vertices.push_back(&meshes[i].vertices[j]);
	}

	for (UINT i = 0; i < materialCount; ++i)
	{
		mat.push_back(mats[i].mat);

		if (mats[i].diffuseMapName != "")
		{
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _diffuseMapSRV = nullptr;
			//std::shared_ptr<ID3D11ShaderResourceView> _diffuseMapSRV = nullptr;
			//_diffuseMapSRV = textureMgr->CreateLevelTexture(mats[i].diffuseMapName);
			_diffuseMapSRV = textureMgr->CreateLevelTexture(mats[i].diffuseMapName);
			//ID3D11ShaderResourceView* _diffuseMapSRV = textureMgr->CreateLevelTexture(mats[i].diffuseMapName);
			diffuseMapSRV.push_back(_diffuseMapSRV);
		}
		else
		{
			diffuseMapSRV.push_back(NULL);
		}

		if (mats[i].normalMapName != "")
		{
			//ID3D11ShaderResourceView* _normalMapSRV = textureMgr->CreateLevelTexture(mats[i].normalMapName);
			//normalMapSRV.push_back(_normalMapSRV);

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

	for (UINT i = 0; i < meshCount; ++i)
	{
		for (UINT j = 0; j < meshes[i].vertices.size(); ++j)
		{
			XMVECTOR pos = XMLoadFloat3(&meshes[i].vertices[j].position);

			vMin = XMVectorMin(vMin, pos);
			vMax = XMVectorMax(vMax, pos);
		}
	}

// 	for (int i = 0; i < vertices.size(); ++i)
// 	{
// 		XMVECTOR pos = XMLoadFloat3(&vertices[i].position);
// 
// 		vMin = XMVectorMin(vMin, pos);
// 		vMax = XMVectorMax(vMax, pos);
// 	}

	// Store values in bounding box
	XMStoreFloat3(&boundingBox.Center, 0.5f*(vMin+vMax));
	XMStoreFloat3(&boundingBox.Extents, 0.5f*(vMax-vMin));

	//ComputeBoundingAxisAlignedBoxFromPoints(&boundingBox, vertices.size(), &vertices[0]->position, sizeof(Vertex::Basic32));
}


GenericModel::~GenericModel(void)
{
	diffuseMapSRV.clear();
	normalMapSRV.clear();
}

Vertex::PosNormalTex* GenericModel::GetVertex(int index)
{
	for (UINT i = 0; i < meshCount; ++i)
	{
		/*int s = meshes[i].indices.size();
		int i2 = index - s;
		if(i2 < 0)
			return &meshes[i].vertices[meshes[i].indices[index]-1];
		index = i2;*/

		int i2 = index - (int)meshes[i].vertices.size();
		if(i2 < 0)
			return &meshes[i].vertices[index];
		index = i2;
	}

	return NULL;
}

int GenericModel::GetVertexCount()
{
	int size = 0;
	/*for (UINT i = 0; i < meshCount; ++i)
		size += meshes[i].indices.size();*/
	for (UINT i = 0; i < meshCount; ++i)
		size += (int)meshes[i].vertices.size();


	return size-10000;
}
