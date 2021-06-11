#pragma once

#include <type_traits>

#include <lua/lua.hpp>

#include <TexRPLib/GPUInterface.h>
#include <TexRPLib/GPUContext.h>
#include <TexRPLib/GPUTextureStack.h>
#include <TexRPLib/GPUGeometryModell.h>
#include <TexRPLib/GPUMask.h>

namespace TexRPCli {
	namespace Lua {
		enum class HandleType : unsigned char {
			GPUNull				= 0x00,
			GPUInterface		= 0x01,
			GPUContext			= 0x02,
			GPUTextureStack		= 0x03,
			GPUGeometryModell	= 0x04,
			GPUMask				= 0x05,
		};

		// Handle that represents a lua object
		class Handle {
			public:
				// Init handle with pointer
				template<typename T, typename NPT = std::remove_pointer_t<T>>
				void init(NPT* pointer) {
					if (pointer) {
						m_cPointer = pointer;
						m_type = TypeToEnumConverter<NPT>::type();
					}
				}

				// Close handle
				void close();
				// Checks if the handle is open
				bool isOpen();
				// Get type
				TexRPCli::Lua::HandleType type();

				// Get pointer for type
				template<typename T, typename NPT = std::remove_pointer_t<T>>
				NPT* get() {
					// Check type and return c pointer
					if (m_type == TypeToEnumConverter<NPT>::type()) {
						return (NPT*)m_cPointer;
					}

					// Fallback
					return nullptr;
				}

				// Level of the handle (0: Highst level ... n: Lowest level)
				int handleLevel = 1;

			private:
				// C++ object pointer
				ITexRPObject* m_cPointer = nullptr;

				// Type of handle
				HandleType m_type = HandleType::GPUNull;

			private:
				// Type to enum converter
				template<typename T>
				class TypeToEnumConverter {
					public: 
						template <typename XT = std::enable_if_t<std::is_same_v<T, TexRPLib::IGPUInterface>>>
						static HandleType type(TexRPLib::IGPUInterface* pType = nullptr) {
							return HandleType::GPUInterface;
						}

						template <typename XT = std::enable_if_t<std::is_same_v<T, TexRPLib::IGPUContext>>>
						static HandleType type(TexRPLib::IGPUContext* pType = nullptr) {
							return HandleType::GPUContext;
						}

						template <typename XT = std::enable_if_t<std::is_same_v<T, TexRPLib::IGPUTextureStack>>>
						static HandleType type(TexRPLib::IGPUTextureStack* pType = nullptr) {
							return HandleType::GPUTextureStack;
						}

						template <typename XT = std::enable_if_t<std::is_same_v<T, TexRPLib::IGPUGeometryModell>>>
						static HandleType type(TexRPLib::IGPUGeometryModell* pType = nullptr) {
							return HandleType::GPUGeometryModell;
						}

						template <typename XT = std::enable_if_t<std::is_same_v<T, TexRPLib::IGPUMask>>>
						static HandleType type(TexRPLib::IGPUMask* pType = nullptr) {
							return HandleType::GPUMask;
						}
				};
		};

		// Binding class to lua
		class LuaHandle {
			public:
				// Bind to lua state
				static void bind(lua_State* ptrState);

			private:
				// Close a handle lua sided
				static int lua_CloseHandle(lua_State* ptrState);
				// Get the handle type for lua
				static int lua_GetHandleType(lua_State* ptrState);
				// Lua side check if handle is valid
				static int lua_CheckHandle(lua_State* ptrState);
		};
	}
}
