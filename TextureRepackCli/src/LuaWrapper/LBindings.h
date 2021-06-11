#pragma once

#include <lua/lua.hpp>

#include <TexRPLib/GPUInterface.h>
#include <TexRPLib/GPUContext.h>
#include <TexRPLib/GPUTextureStack.h>
#include <TexRPLib/GPUGeometryModell.h>
#include <TexRPLib/GPUMask.h>

#include <LuaWrapper/Handles/HandleManager.h>
#include <LuaWrapper/LState.h>

namespace TexRPCli {
	namespace Lua {
		class Bindings {
			public:
				// Bind
				static void bind(lua_State* ptrState);

			private:
				// === IGPUInterface ===
				// nil / handle = EnumGpuInterfaces(int index)
				static int lua_EnumGpuInterfaces(lua_State* ptrState);
				// nil / string = IGPUInterface_GetGPUName(handle)
				static int lua_IGPUInterface_GetGPUName(lua_State* ptrState);
				// nil / int = IGPUInterface_GetGPUUniqueId(handle)
				static int lua_IGPUInterface_GetGPUUniqueId(lua_State* ptrState);
				// nil / int = IGPUInterface_GetGPUFreeVRAM(handle)
				static int lua_IGPUInterface_GetGPUFreeVRAM(lua_State* ptrState);
				// nil / bool = IGPUInterface_SetVRAMReservation(handle, int)
				static int lua_IGPUInterface_SetVRAMReservation(lua_State* ptrState);
				// nil / int = IGPUInterface_GetVRAMReservation(handle)
				static int lua_IGPUInterface_GetVRAMReservation(lua_State* ptrState);
				// nil / handle = IGPUInterface_CreateContex(handle)
				static int lua_IGPUInterface_CreateContex(lua_State* ptrState);

				// === IGPUContext ===
				// nil / handle = IGPUContext_CreateTextureStack(handle)
				static int lua_IGPUContext_CreateTextureStack(lua_State* ptrState);
				// nil / handle = IGPUContext_OpenModell(handle, string)
				static int lua_IGPUContext_OpenModell(lua_State* ptrState);
				//								   OBJECT  STACK   OUTPUT  COUNT   MASKS...   TEXTURS...
				// bool = IGPUContext_MergTextures(handle, handle, int,    int,    handle..., int...)
				static int lua_IGPUContext_MergTextures(lua_State* ptrState);

				// === IGPUGeometryModell ===
				// int = IGPUGeometryModell_GetSubmodellCount(handle)
				static int lua_IGPUGeometryModell_GetSubmodellCount(lua_State* ptrState);
				// nil / string = IGPUGeometryModell_GetSubmodellName(handle, index)
				static int lua_IGPUGeometryModell_GetSubmodellName(lua_State* ptrState);
				// nil / int = IGPUGeometryModell_GetUVSetIndex(handle, index)
				static int lua_IGPUGeometryModell_GetUVSetIndex(lua_State* ptrState);
				// nil / int = IGPUGeometryModell_GetVerticesCount(handle, index)
				static int lua_IGPUGeometryModell_GetVerticesCount(lua_State* ptrState);
				// handle = IGPUGeometryModell_CreateMask(handle, index, int)
				static int lua_IGPUGeometryModell_CreateMask(lua_State* ptrState);

				// === IGPUMask ===
				// void = IGPUMask_Clear(handle)
				static int lua_IGPUMask_Clear(lua_State* ptrState);

				// === IGPUTextureStack ===
				//							   OBJ	   W    H    BPP  COUNT
				// bool IGPUTextureStack_Reset(handle, int, int, int, int)
				static int lua_IGPUTextureStack_Reset(lua_State* ptrState);
				// int IGPUTextureStack_LoadFromDisk(handle, string)
				static int lua_IGPUTextureStack_LoadFromDisk(lua_State* ptrState);
				// int IGPUTextureStack_CreateEmpty(handle, int, string)
				static int lua_IGPUTextureStack_CreateEmpty(lua_State* ptrState);
				// bool IGPUTextureStack_ClearTexture(handle, int)
				static int lua_IGPUTextureStack_ClearTexture(lua_State* ptrState);
				// bool IGPUTextureStack_SafeToDisk(handle, int)
				static int lua_IGPUTextureStack_SafeToDisk(lua_State* ptrState);
				// bool IGPUTextureStack_Rename(handle, int, string)
				static int lua_IGPUTextureStack_Rename(lua_State* ptrState);
		};
	}
}
