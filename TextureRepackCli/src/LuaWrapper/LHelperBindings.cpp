#include "LHelperBindings.h"

// Static instance
int TexRPCli::Lua::HelperBindings::luaReturnCode = 0;
TexRPCli::CommandLineArgs* TexRPCli::Lua::HelperBindings::ptrCmdArgs = nullptr;

void TexRPCli::Lua::HelperBindings::bind(lua_State* ptrState, CommandLineArgs* ptrCmdLineArgs, const char* libVersion, const char* cliVersion) {
	// Set global
	ptrCmdArgs = ptrCmdLineArgs;
	
	// === Version ===
	lua_pushstring(ptrState, libVersion);
	lua_setglobal(ptrState, "TEXRP_LIB_VERSION");
	lua_pushstring(ptrState, cliVersion);
	lua_setglobal(ptrState, "TEXRP_CLI_VERSION");
	
	// === Console ===
	// Functions
	lua_register(ptrState, "ConsoleColor", &TexRPCli::Lua::HelperBindings::lua_ConsoleColor);
	lua_register(ptrState, "ConsoleWriteLine", &TexRPCli::Lua::HelperBindings::lua_ConsoleWriteLine);
	lua_register(ptrState, "ConsoleWrite", &TexRPCli::Lua::HelperBindings::lua_ConsoleWrite);
	lua_register(ptrState, "ConsoleReadLine", &TexRPCli::Lua::HelperBindings::lua_ConsoleReadLine);
	lua_register(ptrState, "ConsoleTitle", &TexRPCli::Lua::HelperBindings::lua_ConsoleTitle);

	// Globals
	lua_pushinteger(ptrState, (long long)TexRPCli::ConsoleColor::BLACK);
	lua_setglobal(ptrState, "CONSOLE_COLOR_BLACK");
	lua_pushinteger(ptrState, (long long)TexRPCli::ConsoleColor::WHITE);
	lua_setglobal(ptrState, "CONSOLE_COLOR_WHITE");
	lua_pushinteger(ptrState, (long long)TexRPCli::ConsoleColor::RED);
	lua_setglobal(ptrState, "CONSOLE_COLOR_RED");
	lua_pushinteger(ptrState, (long long)TexRPCli::ConsoleColor::GREEN);
	lua_setglobal(ptrState, "CONSOLE_COLOR_GREEN");
	lua_pushinteger(ptrState, (long long)TexRPCli::ConsoleColor::BLUE);
	lua_setglobal(ptrState, "CONSOLE_COLOR_BLUE");
	lua_pushinteger(ptrState, (long long)TexRPCli::ConsoleColor::LIGHT_RED);
	lua_setglobal(ptrState, "CONSOLE_COLOR_LIGHT_RED");
	lua_pushinteger(ptrState, (long long)TexRPCli::ConsoleColor::LIGHT_GREEN);
	lua_setglobal(ptrState, "CONSOLE_COLOR_LIGHT_GREEN");
	lua_pushinteger(ptrState, (long long)TexRPCli::ConsoleColor::LIGHT_BLUE);
	lua_setglobal(ptrState, "CONSOLE_COLOR_LIGHT_BLUE");
	lua_pushinteger(ptrState, (long long)TexRPCli::ConsoleColor::ORANGE);
	lua_setglobal(ptrState, "CONSOLE_COLOR_ORANGE");

	// === File ===
	lua_register(ptrState, "FileExists", &TexRPCli::Lua::HelperBindings::lua_FileExists);
	lua_register(ptrState, "FileDelete", &TexRPCli::Lua::HelperBindings::lua_FileDelete);
	lua_register(ptrState, "FileMove", &TexRPCli::Lua::HelperBindings::lua_FileMove);
	lua_register(ptrState, "FileCopy", &TexRPCli::Lua::HelperBindings::lua_FileCopy);

	// === Enviroement ===
	lua_register(ptrState, "GetAppDir", &TexRPCli::Lua::HelperBindings::lua_GetAppDir);
	lua_register(ptrState, "GetWorkDir", &TexRPCli::Lua::HelperBindings::lua_GetWorkDir);
	lua_register(ptrState, "GetUserDir", &TexRPCli::Lua::HelperBindings::lua_GetUserDir);
	lua_register(ptrState, "GetCmdArg", &TexRPCli::Lua::HelperBindings::lua_GetCmdArg);

	// === Return code ===
	lua_register(ptrState, "ReturnCode", &TexRPCli::Lua::HelperBindings::lua_ReturnCode);
}

int TexRPCli::Lua::HelperBindings::getScriptReturnCode() {
	return luaReturnCode;
}

int TexRPCli::Lua::HelperBindings::lua_ConsoleWriteLine(lua_State* ptrState) {
	//Check
	if (lua_isstring(ptrState, 1)) {
		Console::get().writeLine(lua_tostring(ptrState, 1));
	}
	
	// VOID
	return 0;
}

int TexRPCli::Lua::HelperBindings::lua_ConsoleWrite(lua_State* ptrState) {
	//Check
	if (lua_isstring(ptrState, 1)) {
		Console::get().write(lua_tostring(ptrState, 1));
	}

	// VOID
	return 0;
}

int TexRPCli::Lua::HelperBindings::lua_ConsoleReadLine(lua_State* ptrState) {
	// Read line
	CHAR buffer[2048];
	Console::get().readLine(buffer, 2048);
	
	// Push line
	lua_pushstring(ptrState, buffer);
	return 1;
}

int TexRPCli::Lua::HelperBindings::lua_ConsoleColor(lua_State* ptrState) {
	//Check
	if (lua_isinteger(ptrState, 1)) {
		Console::get().setColor((TexRPCli::ConsoleColor)lua_tointeger(ptrState, 1));
	}

	// VOID
	return 0;
}

int TexRPCli::Lua::HelperBindings::lua_ConsoleTitle(lua_State* ptrState) {
	// Check
	if (lua_isstring(ptrState, 1)) {
		Console::get().setTitle(lua_tostring(ptrState, 1));
	}

	// VOID
	return 0;
}

int TexRPCli::Lua::HelperBindings::lua_FileExists(lua_State* ptrState) {
	// Result
	bool fileExitsts = false;
	
	// Check and get path
	if (lua_isstring(ptrState, 1)) {
		// Try open file
		HANDLE hFile = CreateFileA(lua_tostring(ptrState, 1), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
		if (hFile != INVALID_HANDLE_VALUE) {
			// On succeed close and set exitst to true
			CloseHandle(hFile);
			fileExitsts = true;
		}
	}

	// Push and return
	lua_pushboolean(ptrState, fileExitsts);
	return 1;
}

int TexRPCli::Lua::HelperBindings::lua_FileCopy(lua_State* ptrState) {
	// Result
	bool copySucceed = false;
	
	// Check arguments
	if (lua_isstring(ptrState, 1) && lua_isstring(ptrState, 2)) {
		// Copy file
		copySucceed = CopyFileA(lua_tostring(ptrState, 1), lua_tostring(ptrState, 2), FALSE);
	}

	// Push and return
	lua_pushboolean(ptrState, copySucceed);
	return 1;
}

int TexRPCli::Lua::HelperBindings::lua_FileDelete(lua_State* ptrState) {
	// Result
	bool deleteSucceed = false;
	
	// Check
	if (lua_isstring(ptrState, 1)) {
		// Delete file
		deleteSucceed = DeleteFileA(lua_tostring(ptrState, 1));
	}

	// Push and return
	lua_pushboolean(ptrState, deleteSucceed);
	return 1;
}

int TexRPCli::Lua::HelperBindings::lua_FileMove(lua_State* ptrState) {
	// Result
	bool moveSucceed = false;
	
	// Check
	if (lua_isstring(ptrState, 1) && lua_isstring(ptrState, 1)) {
		// Delete file
		moveSucceed = MoveFileA(lua_tostring(ptrState, 1), lua_tostring(ptrState, 2));
	}

	// Push and return
	lua_pushboolean(ptrState, moveSucceed);
	return 1;
}

int TexRPCli::Lua::HelperBindings::lua_GetWorkDir(lua_State* ptrState) {
	// Lua push path
	lua_pushstring(ptrState, TexRPCli::DefaultDirs::getRunDir());
	return 1;
}

int TexRPCli::Lua::HelperBindings::lua_GetAppDir(lua_State* ptrState) {
	// Lua push path
	lua_pushstring(ptrState, TexRPCli::DefaultDirs::getAppDir());
	return 1;
}

int TexRPCli::Lua::HelperBindings::lua_GetUserDir(lua_State* ptrState) {
	// Lua push path
	lua_pushstring(ptrState, TexRPCli::DefaultDirs::getUserDir());
	return 1;
}

int TexRPCli::Lua::HelperBindings::lua_GetCmdArg(lua_State* ptrState) {
	// Check by index
	if (lua_isinteger(ptrState, 1)) {
		// Get and check index
		long long index = lua_tointeger(ptrState, 1);
		if (index >= 0) {
			// Get and check arg
			const char* arg = ptrCmdArgs->getOrderedScriptArg(index);
			if (arg) {
				// Push and return
				lua_pushstring(ptrState, arg);
				return 1;
			} 
		}
	}
	// Check by name
	else if (lua_isstring(ptrState, 1)) {
		// Get and check nmae
		const char* name = lua_tostring(ptrState, 1);
		const char* arg = ptrCmdArgs->getNamedScriptArg(name);
		if (arg) {
			// Push and return
			lua_pushstring(ptrState, arg);
			return 1;
		}
	}

	// Nil error
	lua_pushnil(ptrState);
	return 1;
}

int TexRPCli::Lua::HelperBindings::lua_ReturnCode(lua_State* ptrState) {
	// Check code
	if (lua_isinteger(ptrState, 1)) {
		// Get code
		luaReturnCode = lua_tointeger(ptrState, 1);
	}

	// void
	return 0;
}
