#ifndef GENERICOBJECTLOADER_H_
#define GENERICOBJECTLOADER_H_

#include "d3dUtilities.h"
#include "Vertex.h"
#include "GenericMesh.h"
#include "GenericSkinnedMesh.h"
#include "GenericSkinnedModel.h"

#include "SkinnedData.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Logger.hpp>
#include <assimp/DefaultLogger.hpp>

#include "SkinnedDataSorted.h"

struct GenericMaterial
{
	Material mat;
	std::string diffuseMapName;
	std::string normalMapName;
	std::string name;
};

class GenericObjectLoader
{
public:
	GenericObjectLoader(void);
	~GenericObjectLoader(void);

	bool loadObject(
		const std::string& fileName,
		std::vector<GenericMaterial>& materials,
		std::vector<Mesh>& meshes);

	bool loadSkinnedObject(
		const std::string& fileName,
		std::vector<GenericMaterial>& materials,
		std::vector<GenericSkinnedMesh>& meshes);

	bool loadSkinnedObject(
		const std::string& fileName,
		std::vector<GenericMaterial>& materials,
		std::vector<GenericSkinnedMesh>& meshes,
		SkinnedData& skinnedData);

	bool LoadSkinnedObjectSorted(
		const std::string& fileName,
		std::vector<GenericMaterial>& materials,
		std::vector<GenericSkinnedMesh>& meshes,
		SkinnedDataSorted& skinnedData);

	bool LoadMorphObject(const std::string& fileName,
		std::vector<GenericMaterial>& materials,
		std::vector<Mesh>& meshes);

private:
	void InitMaterial(GenericMaterial* mat);
	void ReadMaterials(const aiScene* scene, std::vector<GenericMaterial>& materials, const std::string &path);

	void ReadBones(aiMesh* mesh, GenericSkinnedMesh& myMesh);
	//void ReadBoneKeyframes(aiNodeAnim* nodeAnim, BoneAnimation& boneAnimation);
	SkinData::Bone* CreateBoneTree(aiNode* node, SkinData::Bone* parent, SkinnedData& skinnedData);
	SkinData::Bone* CreateBoneTree(aiNode* node, SkinData::Bone* parent, SkinnedDataSorted& skinnedData);
	//void CalculateBoneToWorldTransform(SkinData::Bone* child);
	void ReadAnimations(const aiScene* scene, SkinnedData& skinnedData);
	void ReadAnimations(const aiScene* scene, SkinnedDataSorted& skinnedData);

	void SetVertexWeights(GenericSkinnedMesh& myMesh);
};

#endif