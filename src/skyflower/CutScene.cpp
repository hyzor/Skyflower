#include "CutScene.h"
#include <math.h>
// Must be included last!
#include "shared/debug.h"

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
	mCurrentYaw = 0.0f;
	mCurrentPitch = 0.0f;
	done = true;
	this->Register(sh);
	
}
void CutScene::play(string name)
{
	translateYawPitch(mCurrentYaw, mCurrentPitch);
	mCurrentWP = 0;
	mWaypoints.clear();

	std::string func = "cutscene_"+name;
	lua_getglobal(mScriptHandlerPtr->L, func.c_str());

	lua_pcall(mScriptHandlerPtr->L, 0, 0, 0);
	done = false;
}

void CutScene::update(float dt)
{  
	float ymp = mCameraPtr->GetYaw();
	if (mWaypoints.size() > 0)
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
			position = Lerp(position, target._position, dt * target._time);

			float tot = target._yaw - self->mCurrentYaw;
			if (tot > 3.14f)
				tot -= 2 * 3.14f;
			else if (tot < -3.14f)
				tot += 2 * 3.14f;

			if (tot > 0)
				mCurrentYaw += std::abs(tot) * dt * target._time;
			else
				mCurrentYaw -= std::abs(tot) * dt * target._time;

			//mCurrentYaw = Lerp(mCurrentYaw, target._yaw, dt * target._time);
			mCurrentPitch = Lerp(mCurrentPitch, target._pitch, dt*target._time);

			mCameraPtr->SetPosition(position);
			mCameraPtr->Rotate(mCurrentYaw, mCurrentPitch);
		}
	}
	else
		done = true;
}

void CutScene::stop()
{
	this->done = true;
	mCurrentPitch = 0.0f;
	mCurrentYaw = 0.0f;
	mCurrentWP = 0;
	mWaypoints.clear();
}

void CutScene::translateYawPitch(float& yaw, float& pitch)
{
	Vec3 lookY = self->mCameraPtr->GetLook();
	lookY.Y = 0;
	lookY.Normalize();

	yaw = asinf(lookY.X);
	if (lookY.Z > 0)
		yaw = yaw;
	else
		yaw = -yaw + 3.14f;

	pitch = -asinf(self->mCameraPtr->GetLook().Y);
}

int CutScene::AddPoint(lua_State* L)
{
	WayPoint wp;
	int n = lua_gettop(L);

	float yaw = self->mCameraPtr->GetYaw();
	wp._position = Vec3(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
	wp._yaw = (float)(lua_tonumber(L, 4) *(3.14 / 180));
	wp._pitch = (float)(lua_tonumber(L, 5) * (3.14 / 180));
	wp._time = (float)lua_tonumber(L, 6);

	self->mWaypoints.push_back(wp);

	return 0;
}

int CutScene::SetCamera(lua_State *L)
{
	Vec3 pos;
	float yaw = 0.0f;
	float pitch = 0.0f;
	int n = lua_gettop(L);

	pos = Vec3(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
	yaw = (float)(lua_tonumber(L, 4) * (3.14 / 180));
	pitch = (float)(lua_tonumber(L, 5) * (3.14 / 180));

	self->mCameraPtr->SetPosition(pos);
	self->mCameraPtr->Rotate(yaw, pitch);

	return 0;
}

int CutScene::isPlaying(lua_State *L)
{
	lua_pushboolean(L, !self->done);
	return 1;
}

int CutScene::play(lua_State *L)
{
	self->play(lua_tostring(L, 1));
	return 0;
}

int CutScene::getCameraPos(lua_State *L)
{
	Vec3 cameraPos = self->mCameraPtr->GetPosition();
	lua_pushinteger(L, cameraPos.X);
	lua_pushinteger(L, cameraPos.Y);
	lua_pushinteger(L, cameraPos.Z);
	return 3;
}

int CutScene::getYawPitch(lua_State *L)
{
	float yaw, pitch;
	self->translateYawPitch(yaw, pitch);

	yaw = XMConvertToDegrees(yaw);
	pitch = XMConvertToDegrees(pitch);

	lua_pushnumber(L, yaw);
	lua_pushnumber(L, pitch);

	return 2;
}