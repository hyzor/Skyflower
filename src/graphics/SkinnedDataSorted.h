#ifndef SKINNEDDATASORTED_H_
#define SKINNEDDATASORTED_H_

#include "d3dUtilities.h"
#include <map>
#include <assimp/scene.h>
#include <tuple>

#include "SkinnedData.h"

class AnimEvaluatorSorted
{
public:
	AnimEvaluatorSorted() : mLastTime(0.0f), mTicksPerSecond(0.0f), mDuration(0.0f), PlayAnimationForward(true), AnimationIndexer(0) {}
	AnimEvaluatorSorted(const aiAnimation* anim);
	void Evaluate(float time, std::map<std::string, SkinData::Bone*>& bones);
	std::vector<XMFLOAT4X4>& GetTransforms(float dt) { return Transforms[GetFrameIndexAt(dt)]; }
	UINT GetFrameIndexAt(float time);

	float GetDuration(UINT frameStart, UINT frameEnd);

	// Frame interval support
	UINT GetFrameIndexAt(float time, UINT frameStart, UINT frameEnd);
	std::vector<XMFLOAT4X4>& GetTransforms(float dt, UINT frameStart, UINT frameEnd) { return Transforms[GetFrameIndexAt(dt, frameStart, frameEnd)]; }

	// Frame interval & animation forward/backward support
	UINT GetFrameIndexAt(float time, UINT frameStart, UINT frameEnd, bool playAnimForward);

	std::vector<XMFLOAT4X4>& GetTransforms(float dt, UINT frameStart, UINT frameEnd, bool playAnimForward)
	{ return Transforms[GetFrameIndexAt(dt, frameStart, frameEnd, playAnimForward)]; }

	std::string Name;

	// If the animation has no name, it's name will be
	// Animation + AnimationIndexer
	UINT AnimationIndexer;
	std::vector<SkinData::AnimationChannel> Channels;
	float mLastTime, mTicksPerSecond, mDuration;
	bool PlayAnimationForward;
	std::vector<std::tuple<UINT, UINT, UINT>> mLastPositions;
	std::vector<std::vector<XMFLOAT4X4>> Transforms;

	std::vector<std::vector<XMFLOAT4X4>> LowerBodyTransforms;
	std::vector<std::vector<XMFLOAT4X4>> UpperBodyTransforms;
};

class SkinnedDataSorted
{
public:
	SkinnedDataSorted() : Skeleton(0), CurrentAnimIndex(-1) {}
	~SkinnedDataSorted();

	bool SetAnimIndex(UINT animIndex);
	bool SetAnimation(const std::string& name);

	void CalcBoneMatrices();
	void Calculate(float time);
	void UpdateTransforms(SkinData::Bone* node);

	// Get transforms for use in vertex shader
	std::vector<XMFLOAT4X4>& GetTransforms(float dt) { return Animations[CurrentAnimIndex].GetTransforms(dt); }

	// Get transforms (animIndex as a parameter to be able to share SkinnedData)
	std::vector<XMFLOAT4X4>& GetTransforms(float dt, UINT animIndex) { return Animations[animIndex].GetTransforms(dt); }

	// Get transforms (animIndex as a parameter to be able to share SkinnedData) with key frame intervals
	std::vector<XMFLOAT4X4>& GetTransforms(float dt, UINT animIndex, UINT frameStart, UINT frameEnd) { return Animations[animIndex].GetTransforms(dt, frameStart, frameEnd); }

	// Get transforms (animIndex as a parameter to be able to share SkinnedData) with key frame intervals and animation forward/backward
	std::vector<XMFLOAT4X4>& GetTransforms(float dt, UINT animIndex, UINT frameStart, UINT frameEnd, bool playAnimForward) 
	{ return Animations[animIndex].GetTransforms(dt, frameStart, frameEnd, playAnimForward); }

	UINT GetAnimationIndex() const { return CurrentAnimIndex; }
	UINT GetAnimationIndex(const std::string& name);

	float GetAnimationSpeed() const { return Animations[CurrentAnimIndex].mTicksPerSecond; }
	void SetAnimationSpeed(float ticksPerSecond) { Animations[CurrentAnimIndex].mTicksPerSecond = ticksPerSecond; }

	float GetAnimationDuration(UINT animationIndex, UINT frameStart, UINT frameEnd);

	SkinData::Bone* Skeleton; // Root node of scene
	std::map<std::string, SkinData::Bone*> BonesByName;
	std::map<std::string, UINT> BonesToIndex;
	std::map<std::string, UINT> AnimationNameToId;
	std::vector<SkinData::Bone*> Bones;
	std::vector<XMFLOAT4X4> Transforms;

	std::vector<XMFLOAT4X4> UpperBodyTransforms;
	std::vector<XMFLOAT4X4> LowerBodyTransforms;

	std::vector<SkinData::Bone*> LowerBodyBones;
	std::vector<SkinData::Bone*> UpperBodyBones;

	std::vector<AnimEvaluatorSorted> Animations;
	UINT CurrentAnimIndex;
};

#endif