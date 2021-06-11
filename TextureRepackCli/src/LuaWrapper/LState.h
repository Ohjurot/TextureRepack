#pragma once

#include <lua/lua.hpp>

#include <iostream>

namespace TexRPCli {
	namespace Lua {
		// Lua state
		class State {
			public:
				// Construct / Destruct
				State();
				~State();

				// Functions
				static void dumpLuaStack(lua_State* L);

				// Operators
				operator lua_State*();

				// Unused
				State(const State&) = delete;
				State& operator=(const State&) = delete;
			private:
				// Lua state
				lua_State* m_ptrLuaState;
		};
	}
}