#ifndef MORPHMODEL_H_
#define MORPHMODEL_H_

#include "d3dUtilities.h"
#include "TextureManager.h"
#include <DirectXCollision.h>
#include "GenericObjectLoader.h"

struct TargetMesh
{
	std::vector<Mesh> meshes;
	UINT nrOfMeshes;
};

class MorphModel
{
public:
	MorphModel(ID3D11Device* device, TextureManager* textureMgr, const std::string& folderPath, const std::string& morphFileName);
	~MorphModel(void);

	UINT meshCount;
	UINT materialCount;

	std::vector<Material> mat;
	std::vector<ID3D11ShaderResourceView*> diffuseMapSRV;
	std::vector<ID3D11ShaderResourceView*> normalMapSRV;

	//std::vector<Mesh> meshes;
	std::vector<TargetMesh> mTargetModels;

	// Bounding box
	DirectX::BoundingBox boundingBox;

	Vertex::PosNormalTex* GetVertex(int index);
	int GetVertexCount();

	template <typename VertexType>
	void SetVertices(ID3D11Device* device, const VertexType* vertices, UINT count);

	void SetIndices(ID3D11Device* device, const UINT* indices, UINT count);

	std::vector<UINT> mIndices;
	std::vector<Vertex::PosNormalTexTargets4> mVertices;

	void CreatePositions(const std::string& fileName);

	void Draw(ID3D11DeviceContext* dc);

private:
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;
	DXGI_FORMAT mIndexBufferFormat;
	UINT mVertexStride;

};

template <typename VertexType>
void MorphModel::SetVertices(ID3D11Device* device, const VertexType* vertices, UINT count)
{
	ReleaseCOM(mVertexBuffer);

	mVertexStride = sizeof(VertexType);

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexType)* count;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vInitData;
	vInitData.pSysMem = vertices;

	//HR(device->CreateBuffer(&vbd, &vInitData, &mVertexBuffer));
	device->CreateBuffer(&vbd, &vInitData, &mVertexBuffer);
}

struct MorphModelInstance
{
	MorphModel* model;
	XMFLOAT4X4 world;
	bool isVisible;
};

#endif