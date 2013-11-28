#include "ScriptHandler.h"

ScriptHandler* ScriptHandler::instance = nullptr;

ScriptHandler::ScriptHandler()
{
	L = lua_open();
	luaopen_base(L);
	luaopen_table(L);
	luaopen_string(L);
	luaopen_math(L);

	lua_register(L, "Print", Print);
}

ScriptHandler::~ScriptHandler()
{
	lua_close(L);
}

ScriptHandler* ScriptHandler::GetInstance()
{
	if (!instance)
		instance = new ScriptHandler();
	return instance;
}

void ScriptHandler::ReportErrors(lua_State* L, int status)
{
	if (status != 0) 
	{
		std::cout << "-- " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
}

void ScriptHandler::Run(std::string file)
{
	int s = luaL_loadfile(L, file.c_str());

	if (s == 0)
		s = lua_pcall(L, 0, LUA_MULTRET, 0);

	ReportErrors(L, s);
}

lua_State* GetLua();


int ScriptHandler::Print(lua_State* L)
{
	int n = lua_gettop(L);
	if(n > 0)
		std::cout << lua_tostring(L, 1) << std::endl;

	return 0;
}
