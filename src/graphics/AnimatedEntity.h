#ifndef ANIMATEDENTITY_H_
#define ANIMATEDENTITY_H_

#include "GenericSkinnedModel.h"
#include "Camera.h"
#include "ShaderHandler.h"

class AnimatedEntity
{
public:
	AnimatedEntity(GenericSkinnedModel* model, XMFLOAT3 position);
	~AnimatedEntity(void);

	//void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera);
	void Draw(ID3D11DeviceContext* dc, Camera* cam, NormalMappedSkinned* shader, XMMATRIX &world);
	void Draw(ID3D11DeviceContext* dc, Camera* cam, BasicDeferredSkinnedShader* deferredShader, XMMATRIX &world);

	void Update(float dt);

	void SetPosition(XMFLOAT3 pos);
	void RotateXYZ(XMFLOAT3 rot, float yaw, XMVECTOR Up);

	void SetAnimation(UINT index, bool loop);
	void SetAnimationSpeed(UINT index, float speed);

	XMFLOAT3 Position;
	XMFLOAT3 Scale;
	float Rotation;

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

	struct Animation
	{
		Animation(UINT animationType, UINT frameStart, UINT frameEnd, float animationSpeed)
		{
			this->AnimationType = animationType;
			this->FrameStart = frameStart;
			this->FrameEnd = frameEnd;
			this->playForwards = true;
			this->AnimationSpeed = animationSpeed;
		}

		Animation(UINT animationType, UINT frameStart, UINT frameEnd, bool playForwards)
		{
			this->AnimationType = animationType;
			this->FrameStart = frameStart;
			this->FrameEnd = frameEnd;
			this->playForwards = playForwards;
		}

		UINT FrameStart, FrameEnd;
		UINT AnimationType;
		bool playForwards;
		float AnimationSpeed;
	};

	std::vector<Animation> mAnimations;
	UINT mCurAnim;

	GenericSkinnedModelInstance mInstance;
	bool mFirstAnimation;
};

#endif