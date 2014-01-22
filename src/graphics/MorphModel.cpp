#include "MorphModel.h"

#include <iostream>
#include <fstream>

MorphModel::MorphModel(ID3D11Device* device, TextureManager* textureMgr, const std::string& folderPath, const std::string& morphFileName)
{
	std::vector<GenericMaterial> mats;

	mIndexBufferFormat = DXGI_FORMAT_R32_UINT;
	mVertexStride = 0;

	bool failed = false;

	// Load all the individual morph target models
	GenericObjectLoader objectLoader;

	std::string morphFilePath = folderPath + morphFileName;

	// Open corresponding morph file
	std::ifstream morphFile;
	morphFile.open(morphFilePath.c_str(), std::ios::in);

	if (!morphFile.is_open())
	{
		std::stringstream ErrorStream;
		ErrorStream << "Failed to load morph file " << morphFilePath;
		std::string ErrorStreamStr = ErrorStream.str();
		LPCSTR Text = ErrorStreamStr.c_str();
		MessageBoxA(0, Text, 0, 0);
		failed = true;
	}
	else
	{
		std::string morphModel;

		while (morphFile >> morphModel)
		{
			std::string morphModelPath = folderPath + morphModel;
			TargetMesh targetModel;
			//this->AnimInst->CreateAnimation(index, start, end, playForwards != 0);
			objectLoader.loadObject(morphModelPath, mats, targetModel.meshes);
			targetModel.nrOfMeshes = targetModel.meshes.size();
			mTargetModels.push_back(targetModel);
		}

		morphFile.close();
	}

	//objectLoader.loadObject(fileName, mats, meshes);

	//meshCount = (UINT)meshes.size();

	// Models actually got loaded
	if (!failed)
	{
		materialCount = (UINT)mats.size();

		// ASSUME EVERY TARGET MODEL HAS THE SAME NUMBER OF MESHES AND VERTICES

		// For every PosNormalTexTargets4 I need to iterate through
		// all the morph targets and add their vertex data to this vertex

		// Collect all the vertices from every morph target model
		// and combine them in a single vertex array

		// First check for irregularity, all the models must have matching number of
		// vertices & meshes
		UINT baseMeshNrOfMeshes = mTargetModels[0].nrOfMeshes;

		for (UINT i = 0; i < mTargetModels.size(); ++i)
		{
			if (mTargetModels[i].nrOfMeshes > baseMeshNrOfMeshes)
			{
				// Error, some target model has more meshes than the base one
				std::stringstream ErrorStream;
				ErrorStream << "Invalid morph target models!" << std::endl
					<< "Using morph file: " << morphFilePath << std::endl
					<< "The base model has " << baseMeshNrOfMeshes << " meshes" << std::endl
					<< "Target model " << i << " has " << mTargetModels[i].nrOfMeshes << " meshes";
				std::string ErrorStreamStr = ErrorStream.str();
				LPCSTR Text = ErrorStreamStr.c_str();
				MessageBoxA(0, Text, 0, 0);
			}
		}

		// Then set the indices to the indices in base model

		int curTargetModel = 0;

		// Push indices from first model
		for (UINT i = 0; i < mTargetModels[curTargetModel].nrOfMeshes; ++i)
		{
			for (UINT k = 0; k < mTargetModels[curTargetModel].meshes[i].indices.size(); ++k)
			{
				mIndices.push_back(mTargetModels[curTargetModel].meshes[i].indices[k]);
			}
		}

		if (mTargetModels.size() == 1)
		{
			for (UINT i = 0; i < mTargetModels[curTargetModel].nrOfMeshes; ++i)
			{
				for (UINT j = 0; j < mTargetModels[curTargetModel].meshes[i].VertexCount; ++j)
				{
					Vertex::PosNormalTexTargets4 vTargets;

					curTargetModel = 0;
					vTargets.positionBase = mTargetModels[curTargetModel].meshes[i].vertices[j].position;
					vTargets.normalBase = mTargetModels[curTargetModel].meshes[i].vertices[j].normal;

					mVertices.push_back(vTargets);
				}
			}
		}
		else if (mTargetModels.size() == 2)
		{
			for (UINT i = 0; i < mTargetModels[curTargetModel].nrOfMeshes; ++i)
			{
				for (UINT j = 0; j < mTargetModels[curTargetModel].meshes[i].VertexCount; ++j)
				{
					Vertex::PosNormalTexTargets4 vTargets;

					curTargetModel = 0;
					vTargets.positionBase = mTargetModels[curTargetModel].meshes[i].vertices[j].position;
					vTargets.normalBase = mTargetModels[curTargetModel].meshes[i].vertices[j].normal;

					curTargetModel = 1;
					vTargets.positionTarget1 = mTargetModels[curTargetModel].meshes[i].vertices[j].position;
					vTargets.normalTarget1 = mTargetModels[curTargetModel].meshes[i].vertices[j].normal;

					mVertices.push_back(vTargets);
				}
			}
		}

		else if (mTargetModels.size() == 3)
		{
			for (UINT i = 0; i < mTargetModels[curTargetModel].nrOfMeshes; ++i)
			{
				for (UINT j = 0; j < mTargetModels[curTargetModel].meshes[i].VertexCount; ++j)
				{
					Vertex::PosNormalTexTargets4 vTargets;

					curTargetModel = 0;
					vTargets.positionBase = mTargetModels[curTargetModel].meshes[i].vertices[j].position;
					vTargets.normalBase = mTargetModels[curTargetModel].meshes[i].vertices[j].normal;

					curTargetModel = 1;
					vTargets.positionTarget1 = mTargetModels[curTargetModel].meshes[i].vertices[j].position;
					vTargets.normalTarget1 = mTargetModels[curTargetModel].meshes[i].vertices[j].normal;

					curTargetModel = 2;
					vTargets.positionTarget2 = mTargetModels[curTargetModel].meshes[i].vertices[j].position;
					vTargets.normalTarget2 = mTargetModels[curTargetModel].meshes[i].vertices[j].normal;

					mVertices.push_back(vTargets);
				}
			}
		}

		else if (mTargetModels.size() == 4)
		{
			for (UINT i = 0; i < mTargetModels[curTargetModel].nrOfMeshes; ++i)
			{
				for (UINT j = 0; j < mTargetModels[curTargetModel].meshes[i].VertexCount; ++j)
				{
					Vertex::PosNormalTexTargets4 vTargets;

					curTargetModel = 0;
					vTargets.positionBase = mTargetModels[curTargetModel].meshes[i].vertices[j].position;
					vTargets.normalBase = mTargetModels[curTargetModel].meshes[i].vertices[j].normal;

					curTargetModel = 1;
					vTargets.positionTarget2 = mTargetModels[curTargetModel].meshes[i].vertices[j].position;
					vTargets.normalTarget2 = mTargetModels[curTargetModel].meshes[i].vertices[j].normal;

					curTargetModel = 2;
					vTargets.positionTarget3 = mTargetModels[curTargetModel].meshes[i].vertices[j].position;
					vTargets.normalTarget3 = mTargetModels[curTargetModel].meshes[i].vertices[j].normal;

					curTargetModel = 3;
					vTargets.positionTarget4 = mTargetModels[curTargetModel].meshes[i].vertices[j].position;
					vTargets.normalTarget4 = mTargetModels[curTargetModel].meshes[i].vertices[j].normal;

					mVertices.push_back(vTargets);
				}
			}
		}

		else if (mTargetModels.size() == 5)
		{
			for (UINT i = 0; i < mTargetModels[curTargetModel].nrOfMeshes; ++i)
			{
				for (UINT j = 0; j < mTargetModels[curTargetModel].meshes[i].VertexCount; ++j)
				{
					Vertex::PosNormalTexTargets4 vTargets;

					curTargetModel = 0;
					vTargets.positionBase = mTargetModels[curTargetModel].meshes[i].vertices[j].position;
					vTargets.normalBase = mTargetModels[curTargetModel].meshes[i].vertices[j].normal;

					curTargetModel = 1;
					vTargets.positionTarget1 = mTargetModels[curTargetModel].meshes[i].vertices[j].position;
					vTargets.normalTarget1 = mTargetModels[curTargetModel].meshes[i].vertices[j].normal;

					curTargetModel = 2;
					vTargets.positionTarget2 = mTargetModels[curTargetModel].meshes[i].vertices[j].position;
					vTargets.normalTarget2 = mTargetModels[curTargetModel].meshes[i].vertices[j].normal;

					curTargetModel = 3;
					vTargets.positionTarget3 = mTargetModels[curTargetModel].meshes[i].vertices[j].position;
					vTargets.normalTarget3 = mTargetModels[curTargetModel].meshes[i].vertices[j].normal;

					curTargetModel = 4;
					vTargets.positionTarget4 = mTargetModels[curTargetModel].meshes[i].vertices[j].position;
					vTargets.normalTarget4 = mTargetModels[curTargetModel].meshes[i].vertices[j].normal;

					mVertices.push_back(vTargets);
				}
			}
		}
		else
		{
			// Too many target models
		}

		// 	for (UINT j = 0; j < mTargetModels.size(); ++j)
		// 	{
		// 		for (UINT k = 0; k < mTargetModels[j].nrOfMeshes; ++k)
		// 		{
		// 			for (UINT l = 0; l < mTargetModels[j].meshes[k].VertexCount; ++l)
		// 			{
		// 				Vertex::PosNormalTexTargets4 vTargets;
		// 				vTargets.positionBase = mTargetModels[j].meshes[k].vertices[l].position;
		// 				*(XMFLOAT3*)(&(vTargets.positionTarget1) + k * 12) = mTargetModels[j].meshes[k].vertices[l].position;
		// 			}
		// 		}
		// 	}

		/*
		// Set vertices and indices for each mesh
		for (UINT i = 0; i < meshCount; ++i)
		{
		meshes[i].SetIndices(device, &meshes[i].indices[0], (UINT)meshes[i].indices.size());
		meshes[i].SetVertices(device, &meshes[i].vertices[0], (UINT)meshes[i].vertices.size());
		}
		*/

		SetVertices(device, &mVertices[0], (UINT)mVertices.size());
		SetIndices(device, &mIndices[0], (UINT)mIndices.size());

		for (UINT i = 0; i < materialCount; ++i)
		{
			mat.push_back(mats[i].mat);

			if (mats[i].diffuseMapName != "")
			{
				ID3D11ShaderResourceView* _diffuseMapSRV = textureMgr->CreateTexture(mats[i].diffuseMapName);
				diffuseMapSRV.push_back(_diffuseMapSRV);
			}
			else
			{
				diffuseMapSRV.push_back(NULL);
			}

			if (mats[i].normalMapName != "")
			{
				ID3D11ShaderResourceView* _normalMapSRV = textureMgr->CreateTexture(mats[i].normalMapName);
				normalMapSRV.push_back(_normalMapSRV);
			}

			else
			{
				normalMapSRV.push_back(NULL);
			}
		}
	}

	//--------------------------------------------------------
	// Compute bounding box
	//--------------------------------------------------------
	/*
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

	// Store values in bounding box
	XMStoreFloat3(&boundingBox.Center, 0.5f*(vMin + vMax));
	XMStoreFloat3(&boundingBox.Extents, 0.5f*(vMax - vMin));
	*/
}


MorphModel::~MorphModel()
{
}

void MorphModel::Draw(ID3D11DeviceContext* dc)
{
	UINT offset = 0;

	dc->IASetVertexBuffers(0, 1, &mVertexBuffer, &mVertexStride, &offset);
	dc->IASetIndexBuffer(mIndexBuffer, mIndexBufferFormat, 0);

	// The Vertex Shader expects application provided input data (which is to say data other than hardware auto-generated values such as VertexID or InstanceID). Therefore an Input Assembler object is expected, but none is bound. [ EXECUTION ERROR #349: DEVICE_DRAW_INPUTLAYOUT_NOT_SET]
	dc->DrawIndexed(
		(UINT)mIndices.size(),
		0,
		0);
}

void MorphModel::SetIndices(ID3D11Device* device, const UINT* indices, UINT count)
{
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)* count;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;

	device->CreateBuffer(&ibd, &iinitData, &mIndexBuffer);
}
