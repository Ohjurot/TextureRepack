#include <cstdint>
#include <iostream>

#include <lua/lua.hpp>
#include <Util/Console.h>

#include <LuaWrapper/LState.h>

/*
* Cli Usage
* texrp <sysargs...> [SCRIPTNAME] <scriptargs...>
*	sysargs:   [-[NAME]]<:<VALUE>>
*   scripargs: [-[NAME]]<:<VALUE>>		--> NAME		  = VALUE
*   scripargs: [VALUE]					--> 0 ... 1 ... n = VALUE
* 
*	List of system args:
*	-nocolor : Disables colors in the console
*   -silent	 : Disables console output
*/

/*
* TODO:
* - Command line arguments
* - Add return codes
* - Proper error handling
*/

#include <LuaWrapper/Handles/LuaHandle.h>
#include <LuaWrapper/LBindings.h>
#include <LuaWrapper/LHelperBindings.h>

using namespace TexRPCli;

// Application entry point
int main(int argc, char** argv) {
	// Lua State
	Lua::State state;
	Lua::LuaHandle::bind(state);
	Lua::Bindings::bind(state);
	Lua::HelperBindings::bind(state);

	// Load file and run
	luaL_loadfile(state, "test.lua");
	auto res = lua_pcall(state, 0, LUA_MULTRET, 0);

	// Error Handling
	if (res && lua_isstring(state, -1)) {
		Console::get().writeLine(lua_tostring(state, -1));
	}

	// Shutdown handles
	Lua::HandleManager::get().release();

	// Passed OK
	return 0;
}
