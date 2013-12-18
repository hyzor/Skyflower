#include "Character.h"


Character::Character(GenericSkinnedModel* model, XMFLOAT3 position)
{
	mAnimEntity = new AnimatedEntity(model, position);

	// Cache animation keyframes
	mAnimEntity->mAnimations.push_back(AnimatedEntity::Animation(IdleAnim, 51 + 15, 51 + 15));
	mAnimEntity->mAnimations.push_back(AnimatedEntity::Animation(RunningAnim, 1, 24 + 7));
	mAnimEntity->mAnimations.push_back(AnimatedEntity::Animation(RunningBackwardsAnim, 1, 24 + 7));
	mAnimEntity->mAnimations.push_back(AnimatedEntity::Animation(JumpingAnim, 30 + 7, 49 + 10));
	mAnimEntity->mAnimations.push_back(AnimatedEntity::Animation(StrafingRightAnim, 51 + 15, 75 + 20));
	mAnimEntity->mAnimations.push_back(AnimatedEntity::Animation(StrafingLeftAnim, 81 + 20, 105 + 25));

	mAnimEntity->mCurAnim = RunningAnim;
}


Character::~Character()
{
	delete mAnimEntity;
}

// void Character::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* camera)
// {
// 	mAnimEntity->Draw(dc, activeTech, camera);
// }

void Character::Update(float dt)
{
	mAnimEntity->SetKeyFrameInterval(mAnimEntity->mAnimations[mAnimEntity->mCurAnim].FrameStart, mAnimEntity->mAnimations[mAnimEntity->mCurAnim].FrameEnd);
	mAnimEntity->Update(dt);
}

void Character::Draw(ID3D11DeviceContext* dc, Camera* cam, NormalMappedSkinned* shader)
{
	//mAnimEntity->Draw(dc, cam, shader);
}