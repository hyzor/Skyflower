#ifndef SCRIPTHANDLER_H
#define SCRIPTHANDLER_H

#include <iostream>

#include <lua.hpp>

class ScriptHandler
{
	public:
		static ScriptHandler* GetInstance();
		~ScriptHandler();

		void Run(std::string file);
		lua_State* GetLua();

	private:
		ScriptHandler();
		static ScriptHandler* instance;

		void ReportErrors(lua_State* L, int status);

		lua_State* L;

		static int Print(lua_State* L);
};


#endif