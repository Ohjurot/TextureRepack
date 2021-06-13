#include "LBindings.h"

void TexRPCli::Lua::Bindings::bind(lua_State* ptrState) {
	// === IGPUInterface ===
	lua_register(ptrState, "EnumGpuInterfaces", &TexRPCli::Lua::Bindings::lua_EnumGpuInterfaces);
	lua_register(ptrState, "IGPUInterface_CreateContex", &TexRPCli::Lua::Bindings::lua_IGPUInterface_CreateContex);
	lua_register(ptrState, "IGPUInterface_GetGPUFreeVRAM", &TexRPCli::Lua::Bindings::lua_IGPUInterface_GetGPUFreeVRAM);
	lua_register(ptrState, "IGPUInterface_GetGPUName", &TexRPCli::Lua::Bindings::lua_IGPUInterface_GetGPUName);
	lua_register(ptrState, "IGPUInterface_GetGPUUniqueId", &TexRPCli::Lua::Bindings::lua_IGPUInterface_GetGPUUniqueId);
	lua_register(ptrState, "IGPUInterface_GetVRAMReservation", &TexRPCli::Lua::Bindings::lua_IGPUInterface_GetVRAMReservation);
	lua_register(ptrState, "IGPUInterface_SetVRAMReservation", &TexRPCli::Lua::Bindings::lua_IGPUInterface_SetVRAMReservation);

	// === IGPUContext ===
	lua_register(ptrState, "IGPUContext_CreateTextureStack", &TexRPCli::Lua::Bindings::lua_IGPUContext_CreateTextureStack);
	lua_register(ptrState, "IGPUContext_OpenModell", &TexRPCli::Lua::Bindings::lua_IGPUContext_OpenModell);
	lua_register(ptrState, "IGPUContext_MergeTextures", &TexRPCli::Lua::Bindings::lua_IGPUContext_MergTextures);

	// === IGPUGeometryModell ===
	lua_register(ptrState, "IGPUGeometryModell_GetSubmodellCount", &TexRPCli::Lua::Bindings::lua_IGPUGeometryModell_GetSubmodellCount);
	lua_register(ptrState, "IGPUGeometryModell_GetSubmodellName", &TexRPCli::Lua::Bindings::lua_IGPUGeometryModell_GetSubmodellName);
	lua_register(ptrState, "IGPUGeometryModell_GetSubmodellMaterialName", &TexRPCli::Lua::Bindings::lua_IGPUGeometryModell_GetSubmodellMaterialName);
	lua_register(ptrState, "IGPUGeometryModell_GetUVSetIndex", &TexRPCli::Lua::Bindings::lua_IGPUGeometryModell_GetUVSetIndex);
	lua_register(ptrState, "IGPUGeometryModell_GetVerticesCount", &TexRPCli::Lua::Bindings::lua_IGPUGeometryModell_GetVerticesCount);
	lua_register(ptrState, "IGPUGeometryModell_CreateMask", &TexRPCli::Lua::Bindings::lua_IGPUGeometryModell_CreateMask);

	// === IGPUMask ===
	lua_register(ptrState, "IGPUMask_Clear", &TexRPCli::Lua::Bindings::lua_IGPUMask_Clear);

	// === IGPUTextureStack ===
	lua_register(ptrState, "IGPUTextureStack_ClearTexture", &TexRPCli::Lua::Bindings::lua_IGPUTextureStack_ClearTexture);
	lua_register(ptrState, "IGPUTextureStack_CreateEmpty", &TexRPCli::Lua::Bindings::lua_IGPUTextureStack_CreateEmpty);
	lua_register(ptrState, "IGPUTextureStack_LoadFromDisk", &TexRPCli::Lua::Bindings::lua_IGPUTextureStack_LoadFromDisk);
	lua_register(ptrState, "IGPUTextureStack_Rename", &TexRPCli::Lua::Bindings::lua_IGPUTextureStack_Rename);
	lua_register(ptrState, "IGPUTextureStack_Reset", &TexRPCli::Lua::Bindings::lua_IGPUTextureStack_Reset);
	lua_register(ptrState, "IGPUTextureStack_SafeToDisk", &TexRPCli::Lua::Bindings::lua_IGPUTextureStack_SafeToDisk);
}

int TexRPCli::Lua::Bindings::lua_EnumGpuInterfaces(lua_State* ptrState) {
	// Check parameters
	if (lua_isinteger(ptrState, 1)) {
		// Get index
		long long index = lua_tointeger(ptrState, 1);
		if (index >= 0) {
			// Create interface
			TexRPLib::IGPUInterface* ptrInterface = TexRPLib::enumGpuInterfaces(index);
			if (ptrInterface) {
				// Allocate a handle
				Handle* ptrHandle = HandleManager::get().allocate();
				if (ptrHandle) {
					// Init and return handle
					ptrHandle->init<TexRPLib::IGPUInterface>(ptrInterface);
					lua_pushinteger(ptrState, (long long)ptrHandle);
					return 1;
				}
				else {
					// Destroy if no handle was present
					TexRPDestroy(ptrInterface);
				}
			}
		}
	}

	// NIL
	lua_pushnil(ptrState);
	return 1;
}

int TexRPCli::Lua::Bindings::lua_IGPUInterface_GetGPUName(lua_State* ptrState) {
	// Check parameter
	if (lua_isinteger(ptrState, 1)) {
		// Get and check handle
		Handle* ptrHandle = (Handle*)lua_tointeger(ptrState, 1);
		if (ptrHandle) {
			// Get and check inerface 
			TexRPLib::IGPUInterface* ptrInterface = ptrHandle->get<TexRPLib::IGPUInterface>();
			if (ptrInterface) {
				// Push string
				CHAR buffer[1024];
				if (WideCharToMultiByte(CP_UTF8, NULL, ptrInterface->getGPUName(), -1, buffer, 1024, NULL, NULL)) {
					// Push string
					lua_pushstring(ptrState, buffer);
					return 1;
				}
			}
		}
	}

	// NIL
	lua_pushnil(ptrState);
	return 1;
}

int TexRPCli::Lua::Bindings::lua_IGPUInterface_GetGPUUniqueId(lua_State* ptrState) {
	// Check parameter
	if (lua_isinteger(ptrState, 1)) {
		// Get and check handle
		Handle* ptrHandle = (Handle*)lua_tointeger(ptrState, 1);
		if (ptrHandle) {
			// Get and check inerface 
			TexRPLib::IGPUInterface* ptrInterface = ptrHandle->get<TexRPLib::IGPUInterface>();
			if (ptrInterface) {
				// Get and pack luid
				LUID adapterLuid = ptrInterface->getGPUUniqueId();
				long long packLuid = ((long long)(adapterLuid.HighPart) << 32) | adapterLuid.LowPart; 
				// Push and return
				lua_pushinteger(ptrState, packLuid);
				return 1;
			}
		}
	}

	// NIL
	lua_pushnil(ptrState);
	return 1;
}

int TexRPCli::Lua::Bindings::lua_IGPUInterface_GetGPUFreeVRAM(lua_State* ptrState) {
	// Check parameter
	if (lua_isinteger(ptrState, 1)) {
		// Get and check handle
		Handle* ptrHandle = (Handle*)lua_tointeger(ptrState, 1);
		if (ptrHandle) {
			// Get and check inerface 
			TexRPLib::IGPUInterface* ptrInterface = ptrHandle->get<TexRPLib::IGPUInterface>();
			if (ptrInterface) {
				// Push free vram
				lua_pushinteger(ptrState, ptrInterface->getGPUFreeVRAM());
				return 1;
			}
		}
	}

	// NIL
	lua_pushnil(ptrState);
	return 1;
}

int TexRPCli::Lua::Bindings::lua_IGPUInterface_SetVRAMReservation(lua_State* ptrState) {
	// Check parameter
	if (lua_isinteger(ptrState, 1) && lua_isinteger(ptrState, 2)) {
		// Get and check handle
		Handle* ptrHandle = (Handle*)lua_tointeger(ptrState, 1);
		long long reservation = lua_tointeger(ptrState, 2);
		if (ptrHandle && reservation > 0) {
			// Get and check inerface 
			TexRPLib::IGPUInterface* ptrInterface = ptrHandle->get<TexRPLib::IGPUInterface>();
			if (ptrInterface) {
				// Push and return
				lua_pushboolean(ptrState, ptrInterface->setVRAMReservation(reservation));
				return 1;
			}
		}
	}

	// NIL
	lua_pushnil(ptrState);
	return 1;
}

int TexRPCli::Lua::Bindings::lua_IGPUInterface_GetVRAMReservation(lua_State* ptrState) {
	// Check parameter
	if (lua_isinteger(ptrState, 1)) {
		// Get and check handle
		Handle* ptrHandle = (Handle*)lua_tointeger(ptrState, 1);
		if (ptrHandle) {
			// Get and check inerface 
			TexRPLib::IGPUInterface* ptrInterface = ptrHandle->get<TexRPLib::IGPUInterface>();
			if (ptrInterface) {
				// Push free vram
				lua_pushinteger(ptrState, ptrInterface->getVRAMReservation());
				return 1;
			}
		}
	}

	// NIL
	lua_pushnil(ptrState);
	return 1;
}

int TexRPCli::Lua::Bindings::lua_IGPUInterface_CreateContex(lua_State* ptrState) {
	// Check parameter
	if (lua_isinteger(ptrState, 1)) {
		// Get and check handle
		Handle* ptrHandle = (Handle*)lua_tointeger(ptrState, 1);
		if (ptrHandle) {
			// Get and check inerface 
			TexRPLib::IGPUInterface* ptrInterface = ptrHandle->get<TexRPLib::IGPUInterface>();
			if (ptrInterface) {
				// Create context
				TexRPLib::IGPUContext* ptrContext = ptrInterface->createContex();
				if (ptrContext) {
					// Allocate handle
					Handle* ptrContextHandle = HandleManager::get().allocate();
					if (ptrContextHandle) {
						// Bind push and return
						ptrContextHandle->init<TexRPLib::IGPUContext>(ptrContext);
						ptrContextHandle->handleLevel = 2;
						lua_pushinteger(ptrState, (long long)ptrContextHandle);
						return 1;
					}
					else {
						// Free context when no handle is present
						TexRPDestroy(ptrContext);
					}
				}
			}
		}
	}

	// NIL
	lua_pushnil(ptrState);
	return 1;
}

int TexRPCli::Lua::Bindings::lua_IGPUContext_CreateTextureStack(lua_State* ptrState) {
	// Check parameter
	if (lua_isinteger(ptrState, 1)) {
		// Get and check handle
		Handle* ptrHandle = (Handle*)lua_tointeger(ptrState, 1);
		if (ptrHandle) {
			// Get and check inerface 
			TexRPLib::IGPUContext* ptrContext = ptrHandle->get<TexRPLib::IGPUContext>();
			if (ptrContext) {
				// Create texure stack
				TexRPLib::IGPUTextureStack* ptrTextureStack = ptrContext->createTextureStack();
				if (ptrTextureStack) {
					// Allocate handle
					Handle* ptrNewHandle = HandleManager::get().allocate();
					if (ptrNewHandle) {
						// Init and return
						ptrNewHandle->init<TexRPLib::IGPUTextureStack>(ptrTextureStack);
						ptrNewHandle->handleLevel = 3;
						lua_pushinteger(ptrState, (long long)ptrNewHandle);
						return 1;
					}
					else {
						// Free again
						TexRPDestroy(ptrTextureStack);
					}
				}
			}
		}
	}

	// NIL
	lua_pushnil(ptrState);
	return 1;
}

int TexRPCli::Lua::Bindings::lua_IGPUContext_OpenModell(lua_State* ptrState) {
	// Check parameter
	if (lua_isinteger(ptrState, 1) && lua_isstring(ptrState, 2)) {
		// Get and check handle
		Handle* ptrHandle = (Handle*)lua_tointeger(ptrState, 1);
		if (ptrHandle) {
			// Get and check inerface 
			TexRPLib::IGPUContext* ptrContext = ptrHandle->get<TexRPLib::IGPUContext>();
			if (ptrContext) {
				// Create modell
				TexRPLib::IGPUGeometryModell* ptrModell = ptrContext->openModell(lua_tostring(ptrState, 2));
				if (ptrModell) {
					// Allocate handle
					Handle* ptrHandle = HandleManager::get().allocate();
					if (ptrHandle) {
						// Init and return
						ptrHandle->init<TexRPLib::IGPUGeometryModell>(ptrModell);
						ptrHandle->handleLevel = 3;
						lua_pushinteger(ptrState, (long long)ptrHandle);
						return 1;
					}
					else {
						// Free
						TexRPDestroy(ptrModell);
					}
				}
			}
		}
	}
	
	// NIL
	lua_pushnil(ptrState);
	return 1;
}

int TexRPCli::Lua::Bindings::lua_IGPUContext_MergTextures(lua_State* ptrState) {
	// Storage
	TexRPLib::IGPUContext* ptrContext = nullptr;
	TexRPLib::IGPUTextureStack* ptrStack = nullptr;
	TexRPLib::IGPUMask* maskArray[1024];
	unsigned int indexArray[1024];
	unsigned int outputIndex;
	unsigned int inputCount;

	// Evaluate input
	bool luaOk = false;
	if (lua_isinteger(ptrState, 1) && lua_isinteger(ptrState, 2) && lua_isinteger(ptrState, 3) && lua_isinteger(ptrState, 4)) {
		// Lua::State::dumpLuaStack(ptrState);
		
		// Retrive first lua params
		Handle* ptrContextHandle = (Handle*)lua_tointeger(ptrState, 1);
		Handle* ptrTextureHandle = (Handle*)lua_tointeger(ptrState, 2);
		long long llOutIndex = lua_tointeger(ptrState, 3);
		long long llInCount = lua_tointeger(ptrState, 4);
		
		// Check
		if (ptrContextHandle && ptrTextureHandle && llOutIndex >= 0 && llInCount > 0) {
			// Convert
			ptrContext = ptrContextHandle->get<TexRPLib::IGPUContext>();
			ptrStack = ptrTextureHandle->get<TexRPLib::IGPUTextureStack>();
			outputIndex = llOutIndex;
			inputCount = llInCount;

			// Check
			if (ptrContext && ptrStack) {
				// Get parameters
				luaOk = true;
				for (unsigned int i = 0; i < inputCount && luaOk; i++) {
					// Check
					if (lua_isinteger(ptrState, 5 + i) && lua_isinteger(ptrState, 5 + inputCount + i)) {
						// Extract
						Handle* ptrMaskHandle = (Handle*)lua_tointeger(ptrState, 5 + i);
						long long llTextureIndex = lua_tointeger(ptrState, 5 + inputCount + i);

						// Check
						if (ptrMaskHandle && llTextureIndex >= 0) {
							// Convert
							maskArray[i] = ptrMaskHandle->get<TexRPLib::IGPUMask>();
							indexArray[i] = llTextureIndex;

							// Check
							if (!maskArray[i]) {
								luaOk = false;
							}
						}
					}
					else {
						// Invalidate
						luaOk = false;
					}
				}
			}
		}
	}

	// Execute 
	if (luaOk) {
		luaOk = ptrContext->mergTextures(maskArray, ptrStack, outputIndex, inputCount, indexArray);
	}
	
	// Push result
	lua_pushboolean(ptrState, luaOk);
	return 1;
}

int TexRPCli::Lua::Bindings::lua_IGPUGeometryModell_GetSubmodellCount(lua_State* ptrState) {
	// Output value
	unsigned int subCount = 0;

	// Check
	if (lua_isinteger(ptrState, 1)) {
		// Get handle
		Handle* ptrHandle = (Handle*)lua_tointeger(ptrState, 1);
		if (ptrHandle) {
			// Convert handle
			TexRPLib::IGPUGeometryModell* ptrModell = ptrHandle->get<TexRPLib::IGPUGeometryModell>();
			if (ptrModell) {
				// Set
				subCount = ptrModell->getSubmodellCount();
			}
		}
	}

	// Push output
	lua_pushinteger(ptrState, subCount);
	return 1;
}

int TexRPCli::Lua::Bindings::lua_IGPUGeometryModell_GetSubmodellName(lua_State* ptrState) {
	// Check
	if (lua_isinteger(ptrState, 1) && lua_isinteger(ptrState, 2)) {
		// Get handle
		Handle* ptrHandle = (Handle*)lua_tointeger(ptrState, 1);
		long long llIndex = lua_tointeger(ptrState, 2);
		if (ptrHandle) {
			// Convert handle
			TexRPLib::IGPUGeometryModell* ptrModell = ptrHandle->get<TexRPLib::IGPUGeometryModell>();
			if (ptrModell && llIndex >= 0) {
				// Push and return
				lua_pushstring(ptrState, ptrModell->getSubmodellName(llIndex));
				return 1;
			}
		}
	}

	// NIL
	lua_pushnil(ptrState);
	return 1;
}

int TexRPCli::Lua::Bindings::lua_IGPUGeometryModell_GetSubmodellMaterialName(lua_State* ptrState) {
	// Check
	if (lua_isinteger(ptrState, 1) && lua_isinteger(ptrState, 2)) {
		// Get handle
		Handle* ptrHandle = (Handle*)lua_tointeger(ptrState, 1);
		long long llIndex = lua_tointeger(ptrState, 2);
		if (ptrHandle) {
			// Convert handle
			TexRPLib::IGPUGeometryModell* ptrModell = ptrHandle->get<TexRPLib::IGPUGeometryModell>();
			if (ptrModell && llIndex >= 0) {
				// Push and return
				lua_pushstring(ptrState, ptrModell->getSubmodellMatterialName(llIndex));
				return 1;
			}
		}
	}

	// NIL
	lua_pushnil(ptrState);
	return 1;
}

int TexRPCli::Lua::Bindings::lua_IGPUGeometryModell_GetUVSetIndex(lua_State* ptrState) {
	// Check
	if (lua_isinteger(ptrState, 1) && lua_isinteger(ptrState, 2)) {
		// Get handle
		Handle* ptrHandle = (Handle*)lua_tointeger(ptrState, 1);
		long long llIndex = lua_tointeger(ptrState, 2);
		if (ptrHandle && llIndex >= 0) {
			// Convert handle
			TexRPLib::IGPUGeometryModell* ptrModell = ptrHandle->get<TexRPLib::IGPUGeometryModell>();
			if (ptrModell) {
				// Push and return
				lua_pushinteger(ptrState, ptrModell->getUVSetIndex(llIndex));
				return 1;
			}
		}
	}

	// NIL
	lua_pushnil(ptrState);
	return 1;
}

int TexRPCli::Lua::Bindings::lua_IGPUGeometryModell_GetVerticesCount(lua_State* ptrState) {
	// Check
	if (lua_isinteger(ptrState, 1) && lua_isinteger(ptrState, 2)) {
		// Get handle
		Handle* ptrHandle = (Handle*)lua_tointeger(ptrState, 1);
		long long llIndex = lua_tointeger(ptrState, 2);
		if (ptrHandle && llIndex >= 0) {
			// Convert handle
			TexRPLib::IGPUGeometryModell* ptrModell = ptrHandle->get<TexRPLib::IGPUGeometryModell>();
			if (ptrModell) {
				// Push and return
				lua_pushinteger(ptrState, ptrModell->getVerticesCount(llIndex));
				return 1;
			}
		}
	}

	// NIL
	lua_pushnil(ptrState);
	return 1;
}

int TexRPCli::Lua::Bindings::lua_IGPUGeometryModell_CreateMask(lua_State* ptrState) {
	// Check
	if (lua_isinteger(ptrState, 1) && lua_isinteger(ptrState, 2) && lua_isinteger(ptrState, 3)) {
		// Get handle
		Handle* ptrHandle = (Handle*)lua_tointeger(ptrState, 1);
		long long llIndex = lua_tointeger(ptrState, 2);
		long long llSide = lua_tointeger(ptrState, 3);
		if (ptrHandle && llIndex >= 0 && llSide >= 0) {
			// Convert handle
			TexRPLib::IGPUGeometryModell* ptrModell = ptrHandle->get<TexRPLib::IGPUGeometryModell>();
			if (ptrModell) {
				// Create mask
				TexRPLib::IGPUMask* ptrMask = ptrModell->createMask(llIndex, llSide);
				if (ptrMask) {
					// Allocate handle
					Handle* ptrMaskHandle = HandleManager::get().allocate();
					if (ptrMaskHandle) {
						// Set and return
						ptrMaskHandle->init<TexRPLib::IGPUMask>(ptrMask);
						ptrMaskHandle->handleLevel = 4;
						lua_pushinteger(ptrState, (long long)ptrMaskHandle);
						return 1;
					}
					else {
						// Destroy
						TexRPDestroy(ptrMask);
					}
				}
			}
		}
	}

	// NIL
	lua_pushnil(ptrState);
	return 1;
}

int TexRPCli::Lua::Bindings::lua_IGPUMask_Clear(lua_State* ptrState) {
	// Check
	if (lua_isinteger(ptrState, 1)) {
		// Get handle
		Handle* ptrHandle = (Handle*)lua_tointeger(ptrState, 1);
		if (ptrHandle) {
			// Convert handle
			TexRPLib::IGPUMask* ptrMask = ptrHandle->get<TexRPLib::IGPUMask>();
			if (ptrMask) {
				// Clear
				ptrMask->clear();
			}
		}
	}

	// VOID
	return 0;
}

int TexRPCli::Lua::Bindings::lua_IGPUTextureStack_Reset(lua_State* ptrState) {
	// Result
	bool succeed = false;
	
	// Check
	if (lua_isinteger(ptrState, 1) && lua_isinteger(ptrState, 2) && lua_isinteger(ptrState, 3) && lua_isinteger(ptrState, 4) && lua_isinteger(ptrState, 5)) {
		// Extract data
		Handle* ptrHandle = (Handle*)lua_tointeger(ptrState, 1);
		long long imgWidth = lua_tointeger(ptrState, 2);
		long long imgHeight = lua_tointeger(ptrState, 3);
		long long imgBpp = lua_tointeger(ptrState, 4);
		long long imgCount = lua_tointeger(ptrState, 5);

		// Vertify
		if (ptrHandle && imgWidth > 0 && imgHeight > 0 && imgCount > 0 && (imgBpp == 32 || imgBpp == 64)) {
			// Get texture stack
			TexRPLib::IGPUTextureStack* ptrTextureStack = ptrHandle->get<TexRPLib::IGPUTextureStack>();
			if (ptrTextureStack) {
				// Call texture stack and store result
				succeed = ptrTextureStack->reset(imgWidth, imgHeight, imgBpp, imgCount);
			}
		}
	}

	// Push and return succeed
	lua_pushboolean(ptrState, succeed);
	return 1;
}

int TexRPCli::Lua::Bindings::lua_IGPUTextureStack_LoadFromDisk(lua_State* ptrState) {
	// Result value
	UINT outputIndex = UINT_MAX;
	
	// Check
	if (lua_isinteger(ptrState, 1) && lua_isstring(ptrState, 2)) {
		// Get handle
		Handle* ptrHandle = (Handle*)lua_tointeger(ptrState, 1);
		if (ptrHandle) {
			// Convert handle
			TexRPLib::IGPUTextureStack* ptrTextureStack = ptrHandle->get<TexRPLib::IGPUTextureStack>();
			if (ptrTextureStack) {
				// Clear
				outputIndex = ptrTextureStack->loadFromDisk(lua_tostring(ptrState, 2));
			}
		}
	}

	// Return value
	if (outputIndex == UINT_MAX) {
		lua_pushnil(ptrState);
	}
	else {
		lua_pushinteger(ptrState, outputIndex);
	}

	// one return value
	return 1;
}

int TexRPCli::Lua::Bindings::lua_IGPUTextureStack_CreateEmpty(lua_State* ptrState) {
	// Result value
	UINT outputIndex = UINT_MAX;

	// Check
	if (lua_isinteger(ptrState, 1) && lua_isinteger(ptrState, 2) && lua_isstring(ptrState, 3)) {
		// Get handle
		Handle* ptrHandle = (Handle*)lua_tointeger(ptrState, 1);
		long long llIndex = lua_tointeger(ptrState, 2);
		if (ptrHandle && llIndex >= 0) {
			// Convert handle
			TexRPLib::IGPUTextureStack* ptrTextureStack = ptrHandle->get<TexRPLib::IGPUTextureStack>();
			if (ptrTextureStack) {
				// Clear
				outputIndex = ptrTextureStack->createEmpty(lua_tostring(ptrState, 3), llIndex);
			}
		}
	}

	// Return value
	if (outputIndex == UINT_MAX) {
		lua_pushnil(ptrState);
	}
	else {
		lua_pushinteger(ptrState, outputIndex);
	}

	// one return value
	return 1;
}

int TexRPCli::Lua::Bindings::lua_IGPUTextureStack_ClearTexture(lua_State* ptrState) {
	// Result
	bool succeed = false;
	
	// Check
	if (lua_isinteger(ptrState, 1) && lua_isinteger(ptrState, 2)) {
		// Get handle
		Handle* ptrHandle = (Handle*)lua_tointeger(ptrState, 1);
		long long llIndex = lua_tointeger(ptrState, 2);
		if (ptrHandle && llIndex >= 0) {
			// Convert handle
			TexRPLib::IGPUTextureStack* ptrTextureStack = ptrHandle->get<TexRPLib::IGPUTextureStack>();
			if (ptrTextureStack) {
				// Clear
				succeed = ptrTextureStack->clearTexture(llIndex);
			}
		}
	}

	// Return the results
	lua_pushboolean(ptrState, succeed);
	return 1;
}

int TexRPCli::Lua::Bindings::lua_IGPUTextureStack_SafeToDisk(lua_State* ptrState) {
	// Result
	bool succeed = false;

	// Check
	if (lua_isinteger(ptrState, 1) && lua_isinteger(ptrState, 2)) {
		// Get handle
		Handle* ptrHandle = (Handle*)lua_tointeger(ptrState, 1);
		long long llIndex = lua_tointeger(ptrState, 2);
		if (ptrHandle && llIndex >= 0) {
			// Convert handle
			TexRPLib::IGPUTextureStack* ptrTextureStack = ptrHandle->get<TexRPLib::IGPUTextureStack>();
			if (ptrTextureStack) {
				// Clear
				succeed = ptrTextureStack->safeToDisk(llIndex);
			}
		}
	}

	// Return the results
	lua_pushboolean(ptrState, succeed);
	return 1;
}

int TexRPCli::Lua::Bindings::lua_IGPUTextureStack_Rename(lua_State* ptrState) {
	// Result
	bool succeed = false;

	// Check
	if (lua_isinteger(ptrState, 1) && lua_isinteger(ptrState, 2) && lua_isstring(ptrState, 3)) {
		// Get handle
		Handle* ptrHandle = (Handle*)lua_tointeger(ptrState, 1);
		long long llIndex = lua_tointeger(ptrState, 2);
		if (ptrHandle && llIndex >= 0) {
			// Convert handle
			TexRPLib::IGPUTextureStack* ptrTextureStack = ptrHandle->get<TexRPLib::IGPUTextureStack>();
			if (ptrTextureStack) {
				// Clear
				succeed = ptrTextureStack->rename(llIndex, lua_tostring(ptrState, 3));
			}
		}
	}

	// Return the results
	lua_pushboolean(ptrState, succeed);
	return 1;
}
