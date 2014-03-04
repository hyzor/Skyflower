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
	cameraSet = false;
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
	if (cameraSet)
	{
		mCameraPtr->Rotate(setYaw, setPitch);
		mCameraPtr->SetPosition(setPos);
		mCurrentYaw = setYaw;
		mCurrentPitch = setPitch;
		cameraSet = false;
	}
	float ymp = mCameraPtr->GetYaw();
	if (mWaypoints.size() > 0)
	{
		WayPoint target = mWaypoints.at(mCurrentWP);
		float percentLeft = 1.0f;
		float distance = (mCameraPtr->GetPosition() - target.position).Length();
		if (mCurrentWP > 0)
		{
			percentLeft = distance / (mWaypoints[mCurrentWP - 1].position - target.position).Length();
		}

		if (percentLeft < 0.05f || distance < 5.0f) // go to next wp if less than 5% left
		{
			mCurrentWP++;
			if ((unsigned)mCurrentWP >= mWaypoints.size())
			{
				this->clear();
			}
		}

		if (!done)
		{
			Vec3 position = mCameraPtr->GetPosition();
			position = Lerp(position, target.position, dt * target.time);

			if (self->mCurrentYaw < - 3.14f)
				self->mCurrentYaw += 3.14f*2;
			else if (self->mCurrentYaw > 3.14f)
				self->mCurrentYaw -= 3.14f*2;

			float fR1 = self->mCurrentYaw - target.yaw;
			float fR2 = target.yaw - self->mCurrentYaw;
			if (fR1 > 3.14f)
				fR1 -= 2 * 3.14f;
			if (fR2 < 0.0f)
				fR2 += 2 * 3.14f;

			if (fR2 < fR1)
				mCurrentYaw += fR2 * dt * target.time;
			else
				mCurrentYaw -= fR1 * dt * target.time;

			mCurrentPitch = Lerp(mCurrentPitch, target.pitch, dt*target.time);

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
	wp.position = Vec3(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
	wp.yaw = (float)(lua_tonumber(L, 4) *(3.14 / 180));
	wp.pitch = (float)(lua_tonumber(L, 5) * (3.14 / 180));
	wp.time = (float)lua_tonumber(L, 6);

	self->mWaypoints.push_back(wp);

	return 0;
}

int CutScene::SetCamera(lua_State *L)
{
	self->cameraSet = true;
	int n = lua_gettop(L);

	self->setPos = Vec3(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
	self->setYaw = (float)(lua_tonumber(L, 4) * (3.14 / 180));
	self->setPitch = (float)(lua_tonumber(L, 5) * (3.14 / 180));

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
	lua_pushinteger(L, (lua_Integer)cameraPos.X);
	lua_pushinteger(L, (lua_Integer)cameraPos.Y);
	lua_pushinteger(L, (lua_Integer)cameraPos.Z);
	return 3;
}

int CutScene::getCameraYawPitch(lua_State *L)
{
	float yaw, pitch;
	self->translateYawPitch(yaw, pitch);

	yaw = XMConvertToDegrees(yaw);
	pitch = XMConvertToDegrees(pitch);

	lua_pushnumber(L, yaw);
	lua_pushnumber(L, pitch);

	return 2;
}

void CutScene::clear()
{
	done = true;
	mCurrentPitch = 0.0f;
	mCurrentYaw = 0.0f;
	mCurrentWP = 0;
	mWaypoints.clear();
}