#include "CutScene.h"

CutScene* CutScene::self = nullptr;

template <typename T>
static T Lerp(T a, T b, float amount)
{
	return a + (b - a) * amount;
}

CutScene::CutScene(ScriptHandler* sh, CameraController* camera)
{
	self = this;
	this->mCurrentWP = 0;
	mScriptHandlerPtr = sh;
	mCameraPtr = camera;
	done = true;
	this->Register(sh);
	
}
void CutScene::play()
{
	std::string func = "intro";
	lua_getglobal(mScriptHandlerPtr->L, func.c_str());

	lua_pcall(mScriptHandlerPtr->L, 0, 0, 0);
	done = false;
}
int CutScene::AddPoint(lua_State* L)
{
	WayPoint wp;
	int n = lua_gettop(L);

	wp._position = Vec3(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
	wp._yaw = (float)lua_tonumber(L, 4) * (3.14 / 180);
	wp._pitch = (float)lua_tonumber(L, 5) * (3.14 / 180);
	wp._time = (float)lua_tonumber(L, 6);
	
	self->mWaypoints.push_back(wp);

	return 0;
}

void CutScene::update(float dt)
{   
	WayPoint target = mWaypoints.at(mCurrentWP);
	float distance = (mCameraPtr->GetPosition() - target._position).Length();
	if (distance < 5.0f)
	{
		mCurrentWP++;
		if ((unsigned)mCurrentWP >= mWaypoints.size())
		{
			done = true;
			mCurrentPitch = 0.0f;
			mCurrentYaw = 0.0f;
			mCurrentWP = 0;
			mWaypoints.clear();
		}
	}

	if (!done)
	{
		Vec3 position = mCameraPtr->GetPosition();
		position = Lerp(position, target._position, dt / target._time);
		mCurrentYaw = Lerp(mCurrentYaw, target._yaw, dt / target._time);
		mCurrentPitch = Lerp(mCurrentPitch, target._pitch, dt/target._time);

		mCameraPtr->SetPosition(position);
		mCameraPtr->Rotate(mCurrentYaw, mCurrentPitch);
	}
}

void CutScene::quit()
{
	this->done = true;
}

