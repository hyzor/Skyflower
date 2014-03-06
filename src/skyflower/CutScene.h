#ifndef CUTSCENE_H
#define CUTSCENE_H

#include "ScriptHandler.h"
#include "EntityManager.h"
#include "shared\Vec3.h"
#include "graphics\CameraController.h"

class CutScene
{
public:
	struct WayPoint
	{
		Vec3 position;
		float yaw;
		float pitch;
		float time;
		WayPoint() { this->position = Vec3(); this->yaw = 0.0f; this->pitch = 0.0f; this->time = 0.0f; }
		
		WayPoint(Vec3 position, float yaw, float pitch, float time)
			: position(position), yaw(yaw), pitch(pitch), time(time) {}
	};

	CutScene(ScriptHandler* sh, CameraController* camera);
	void play(string name);

	static void Register(ScriptHandler* sh)
	{
		lua_register(sh->L, "AddPoint", CutScene::AddPoint);
		lua_register(sh->L, "SetCamera", CutScene::SetCamera);
		lua_register(sh->L, "CutScenePlay", CutScene::play);
		lua_register(sh->L, "CutSceneIsPlaying", CutScene::isPlaying);
		lua_register(sh->L, "GetCameraPos", CutScene::getCameraPos);
		lua_register(sh->L, "GetYawPitch", CutScene::getCameraYawPitch);
	};

	void update(float dt);
	
	bool isPlaying() const { return !done; }
	void clear();
	void stop();
public:
	static CutScene* self;

private:
	vector<WayPoint> mWaypoints;
	ScriptHandler* mScriptHandlerPtr;
	CameraController* mCameraPtr;

	bool cameraSet;
	bool done;

	Vec3 setPos;

	float setYaw;
	float setPitch;
	float mCurrentYaw;
	float mCurrentPitch;

	int mCurrentWP;

	void translateYawPitch(float& yaw, float& pitch);

private:
	static int AddPoint(lua_State* L);
	static int SetCamera(lua_State* L);
	static int isPlaying(lua_State *L);
	static int play(lua_State *L);
	static int getCameraPos(lua_State *L);
	static int getCameraYawPitch(lua_State *L);
};



#endif