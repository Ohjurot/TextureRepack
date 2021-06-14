#include <cstdint>
#include <iostream>

#include <lua/lua.hpp>
#include <Util/Console.h>

#include <LuaWrapper/LState.h>

#define TEXRP_CLI_VERSION "TexRPCli V 0.2"

/*
* Cli Usage
* texrp <sysargs...> [SCRIPTNAME] <scriptargs...>
*	sysargs:   [-[NAME]]
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
* (- Advanced error handling)
*/

#include <LuaWrapper/Handles/LuaHandle.h>
#include <LuaWrapper/LBindings.h>
#include <LuaWrapper/LHelperBindings.h>

#include <Util/CommandLineArgs.h>
#include <Util/Console.h>
#include <Util/FileFinder.h>


using namespace TexRPCli;

// Application entry point
int main(int argc, char** argv) {
	// Parse the command line
	CommandLineArgs args(argc, argv);
	if (!args.getScriptName()) {
		Console::get().writeLine("Usage: texrp <system_args> [script_name] <script_args>");
		return -1;
	}
	
	// System setup
	Console::get().setOutputEnable(!args.checkSystemArg("silent"));
	Console::get().setColorEnable(!args.checkSystemArg("nocolor"));

	// Find file
	FileFinder f("paths.txt");
	CHAR scriptPath[4096];
	if (!f.findFile(args.getScriptName(), scriptPath, 4096)) {
		Console::get().write("Script \"");
		Console::get().write(args.getScriptName());
		Console::get().writeLine("\" not found!");
		return -1;
	}

	// Lua State
	Lua::State state;
	Lua::LuaHandle::bind(state);
	Lua::Bindings::bind(state);
	Lua::HelperBindings::bind(state, &args, TEXRP_LIB_VERSION, TEXRP_CLI_VERSION);

	// Load file and run
	luaL_loadfile(state, scriptPath);
	auto res = lua_pcall(state, 0, LUA_MULTRET, 0);

	// Error Handling
	if (res && lua_isstring(state, -1)) {
		Console::get().writeLine(lua_tostring(state, -1));
	}

	// Shutdown handles
	Lua::HandleManager::get().release();

	// Passed OK
	return Lua::HelperBindings::getScriptReturnCode();
}
