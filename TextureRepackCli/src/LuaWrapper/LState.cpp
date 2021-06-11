#include "LState.h"

TexRPCli::Lua::State::State() {
	// Create new lua state
	m_ptrLuaState = luaL_newstate();

	// Load libs
	/*
	luaopen_base(m_ptrLuaState);
	luaopen_coroutine(m_ptrLuaState);
	luaopen_math(m_ptrLuaState);
	luaopen_package(m_ptrLuaState);
	luaopen_string(m_ptrLuaState);
	luaopen_table(m_ptrLuaState);
	luaopen_utf8(m_ptrLuaState);
	*/
	luaL_openlibs(m_ptrLuaState);
}

TexRPCli::Lua::State::~State() {
	// close and unset check
	lua_close(m_ptrLuaState);
	m_ptrLuaState = nullptr;
}

void TexRPCli::Lua::State::dumpLuaStack(lua_State* L) {
	// Get top of lua stack
	int top = lua_gettop(L);
	// For each lua element
	for (unsigned int i = 1; i <= top; i++) {
		// Show index and type
		std::cout << i << "\t" << luaL_typename(L, i);
		// Switch on type
		switch (lua_type(L, i)) {
			case LUA_TNUMBER:
				std::cout << lua_tonumber(L, i) << std::endl;
				break;
			case LUA_TSTRING:
				std::cout << lua_tostring(L, i) << std::endl;
				break;
			case LUA_TBOOLEAN:
				std::cout << (lua_toboolean(L, i) ? "true" : "false");
				break;
			case LUA_TNIL:
				std::cout << "nil" << std::endl;
				break;
			default:
				std::cout << "unknown" << std::endl;
				break;
		}
	}
}

TexRPCli::Lua::State::operator lua_State* () {
	return m_ptrLuaState;
}
