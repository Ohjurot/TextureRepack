#include "LuaHandle.h"

void TexRPCli::Lua::Handle::close() {
	// Free
	if (m_cPointer) {
		TexRPDestroy(m_cPointer);
		m_cPointer = nullptr;
		m_type = HandleType::GPUNull;
	}
}

bool TexRPCli::Lua::Handle::isOpen() {
	return m_cPointer != nullptr;
}

TexRPCli::Lua::HandleType TexRPCli::Lua::Handle::type() {
	return m_type;
}

void TexRPCli::Lua::LuaHandle::bind(lua_State* ptrState){
	lua_register(ptrState, "CloseHandle", &TexRPCli::Lua::LuaHandle::lua_CloseHandle);
	lua_register(ptrState, "GetHandleType", &TexRPCli::Lua::LuaHandle::lua_GetHandleType);
	lua_register(ptrState, "CheckHandle", &TexRPCli::Lua::LuaHandle::lua_CheckHandle);
}

int TexRPCli::Lua::LuaHandle::lua_CloseHandle(lua_State* ptrState) {
	// Get handle pointer
	if (lua_isinteger(ptrState, 1)) { 
		Handle* ptrHandle = (Handle*)lua_tointeger(ptrState, 1);
		if (ptrHandle) {
			// Close handle
			ptrHandle->close();
		}
	}
	
	// Void no return
	return 0;
}

int TexRPCli::Lua::LuaHandle::lua_GetHandleType(lua_State* ptrState) {
	// Get handle pointer
	if (lua_isinteger(ptrState, 1)) {
		Handle* ptrHandle = (Handle*)lua_tointeger(ptrState, 1);
		if (ptrHandle) {
			// Switch on handle type
			switch (ptrHandle->type()) {
				case HandleType::GPUNull:
					lua_pushstring(ptrState, "Null Handle");
					return 1;
				case HandleType::GPUInterface:
					lua_pushstring(ptrState, "GPUInterface");
					return 1;
				case HandleType::GPUContext:
					lua_pushstring(ptrState, "GPUContext");
					return 1;
				case HandleType::GPUTextureStack:
					lua_pushstring(ptrState, "GPUTextureStack");
					return 1;
				case HandleType::GPUGeometryModell:
					lua_pushstring(ptrState, "GPUGeometryModell");
					return 1;
				case HandleType::GPUMask:
					lua_pushstring(ptrState, "GPUMask");
					return 1;
			}
		}
	}
	
	// Push nil
	lua_pushnil(ptrState);
	return 1;
}

int TexRPCli::Lua::LuaHandle::lua_CheckHandle(lua_State* ptrState) {
	// Return value
	bool returnValue = false;

	// Check handle
	if (lua_isinteger(ptrState, 1)) {
		Handle* ptrHandle = (Handle*)lua_tointeger(ptrState, 1);
		if (ptrHandle && ptrHandle->isOpen()) {
			returnValue = true;
		}
	}
	
	// Return value
	lua_pushboolean(ptrState, returnValue);
	return 1;
}
