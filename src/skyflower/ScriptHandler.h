#ifndef SCRIPTHANDLER_H
#define SCRIPTHANDLER_H

#include <iostream>
#include <string>
using namespace std;

#include <lua.hpp>

class ScriptHandler
{
	public:
		static ScriptHandler* GetInstance();
		~ScriptHandler();

		void Run(std::string file);
		lua_State* GetLua();
		bool getMessage();


		lua_State* L;

	private:
		ScriptHandler();
		static ScriptHandler* instance;

		void ReportErrors(lua_State* L, int status);


		static int Print(lua_State* L);


};


#endif