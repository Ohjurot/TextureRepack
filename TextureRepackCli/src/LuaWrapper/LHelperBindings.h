#pragma once

#include <lua/lua.hpp>
#include <Util/Console.h>
#include <Util/DefaultDirs.h>
#include <Util/CommandLineArgs.h>

namespace TexRPCli {
	namespace Lua {
		class HelperBindings {
			public:
				// Bind function
				static void bind(lua_State* ptrState, CommandLineArgs* ptrCmdLineArgs, const char* libVersion, const char* cliVersion);

				// Retrives the scripts return code
				static int getScriptReturnCode();

			private:
				// === Console ===
				// void ConsoleWriteLine(string)
				static int lua_ConsoleWriteLine(lua_State* ptrState);
				// void ConsoleWrite(string)
				static int lua_ConsoleWrite(lua_State* ptrState);
				// string ConsoleReadLine(void)
				static int lua_ConsoleReadLine(lua_State* ptrState);
				// void ConsoleColor(int)
				static int lua_ConsoleColor(lua_State* ptrState);
				// void ConsoleTitle(string)
				static int lua_ConsoleTitle(lua_State* ptrState);

				// === File ===
				// bool FileExists(string)
				static int lua_FileExists(lua_State* ptrState);
				// bool FileCopy(string, string)
				static int lua_FileCopy(lua_State* ptrState);
				// bool FileDelete(string)
				static int lua_FileDelete(lua_State* ptrState);
				// bool MoveRename(string, string)
				static int lua_FileMove(lua_State* ptrState);

				// === Enviroment ===
				// string GetWorkDir()
				static int lua_GetWorkDir(lua_State* ptrState);
				// string GetAppDir()
				static int lua_GetAppDir(lua_State* ptrState);
				// string GetUserDir()
				static int lua_GetUserDir(lua_State* ptrState);
				// nil / string GetCmdArg(int / string)
				static int lua_GetCmdArg(lua_State* ptrState);

				// === Return code ===
				// void ReturnCode(int)
				static int lua_ReturnCode(lua_State* ptrState);

				// === STATIC MEMBERS ===
				// Return code from lua application
				static int luaReturnCode;
				// Command line args pointer
				static CommandLineArgs* ptrCmdArgs;
		};
	}
}