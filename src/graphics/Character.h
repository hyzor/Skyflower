#pragma once

#include "AnimatedEntity.h"

class Character
{
public:
	Character(GenericSkinnedModel* model, XMFLOAT3 position);
	~Character();

	//-------------------------------------------
	// Animations
	//-------------------------------------------
	enum AnimationTypes
	{
		IdleAnim = 0,
		RunningAnim,
		RunningBackwardsAnim,
		JumpingAnim,
		StrafingRightAnim,
		StrafingLeftAnim
	};

	void Update(float dt);
	//void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* camera);
	void Draw(ID3D11DeviceContext* dc, Camera* cam, NormalMappedSkinned* shader);

private:
	AnimatedEntity* mAnimEntity;
};

