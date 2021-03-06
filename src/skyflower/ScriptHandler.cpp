#include "ScriptHandler.h"

// Must be included last!
#include "shared/debug.h"

ScriptHandler::ScriptHandler()
{
	L = luaL_newstate();
	luaL_openlibs(L);

	lua_register(L, "Print", Print);
}

ScriptHandler::~ScriptHandler()
{
	lua_close(L);
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
	std::string path = "../../scripts/" + file;

	int s = luaL_loadfile(L, path.c_str());

	if (s == 0)
		s = lua_pcall(L, 0, LUA_MULTRET, 0);

	ReportErrors(L, s);
}

void ScriptHandler::Load(std::string file)
{
	std::string path = "../../scripts/" + file;

	int s = luaL_loadfile(L, path.c_str());
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

bool ScriptHandler::getMessage()
{
	lua_getglobal(L, "i"); // H�mta globala v�rdet "i" fr�n lua
	int i = (int)lua_tointeger(L, 1);
	lua_pop(L, 1);
	cout << "Variablen i fr�n lua �r = " << i << endl;
	if (i = 12)
	{
		return true;
	}
	else
	{
		return false;
	}



}
