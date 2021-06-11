#pragma once

#include <LuaWrapper/Handles/LuaHandle.h>

#include <algorithm>

#define MAX_HANDLES 4096

namespace TexRPCli {
	namespace Lua {
		// Handle manager
		class HandleManager {
			// Singleton
			public:
				static HandleManager& get();

			private:
				HandleManager() = default;
				~HandleManager();

				static HandleManager s_instance;

			// Class
			public:
				// Allocate a handle 
				TexRPCli::Lua::Handle* allocate();
				void release();

			private:
				// Array of handled
				Handle m_arrHandles[MAX_HANDLES] = {};
				
				// Handle peek position
				int m_peekPos = 0;
		};
	}
}