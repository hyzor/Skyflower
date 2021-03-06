//---------------------------------------------------------------------------------------
// File: BasicModel.h
//
// This class contains a MeshGeometry instance (specifying the geometry of the model).
// It also contains the materials needed, and it contains CPU copies of the mesh data.
//---------------------------------------------------------------------------------------

#ifndef GENERICSKINNEDMODELSORTED_H_
#define GENERICSKINNEDMODELSORTED_H_

#include "d3dUtilities.h"
#include "TextureManager.h"
//#include "xnacollision.h"
#include <DirectXCollision.h>
#include "GenericObjectLoader.h"
#include "SkinnedData.h"
#include "SkinnedDataSorted.h"

class GenericSkinnedModelSorted
{
public:
	GenericSkinnedModelSorted(ID3D11Device* device, TextureManager* textureMgr, const std::string& fileName);
	~GenericSkinnedModelSorted(void);

	UINT numMeshes;
	UINT numMaterials;
	UINT numBones;

	std::vector<Material> mat;
	//std::vector<ID3D11ShaderResourceView*> diffuseMapSRV;
	//std::vector<ID3D11ShaderResourceView*> normalMapSRV;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> diffuseMapSRV;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> normalMapSRV;

	std::vector<GenericSkinnedMesh> meshes;

	std::vector<UINT> mGlobalMaterialIndex;

	// Bounding box
	DirectX::BoundingBox boundingBox;

	SkinnedDataSorted skinnedData;
};

struct GenericSkinnedModelSortedInstance
{
	GenericSkinnedModelSorted* model;
	XMFLOAT4X4 world;
	bool isVisible;

	XMFLOAT4X4 prevWorld;

	//std::vector<XMFLOAT4X4> FinalTransforms;

	std::vector<XMFLOAT4X4> FinalLowerBodyTransforms;
	std::vector<XMFLOAT4X4> FinalUpperBodyTransforms;

	std::string AnimationName;
	UINT AnimationIndex;
	//float TimePos;

	UINT lowerBodyFrameStart, lowerBodyFrameEnd;
	bool playLowerBodyAnimForward;
	bool loopLowerBodyAnim;
	float lowerAnimationSpeed;
	bool lowerAnimationDone;
	float lowerTimePos;

	UINT upperBodyFrameStart, upperBodyFrameEnd;
	bool playUpperBodyAnimForward;
	bool loopUpperbodyAnim;
	float upperAnimationSpeed;
	bool upperAnimationDone;
	float upperTimePos;

	GenericSkinnedModelSortedInstance();
	~GenericSkinnedModelSortedInstance();
	void Update(float dt);
};

#endif