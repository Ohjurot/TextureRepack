#pragma once

#include <TexRPLib/TEXRP.h>
#include <TexRPLib/WinInclude.h>
#include <TexRPLib/GPUContext.h>

namespace TexRPLib {
	class IGPUInterface {
		public:
			// Retrive the name of the gpu
			virtual const wchar_t* getGPUName() = 0;

			// Retrive the gpu's unquie id
			virtual LUID getGPUUniqueId() = 0;

			// Retrive the current free gpu ram
			virtual UINT64 getGPUFreeVRAM() = 0;

			// Create GPU Contex
			virtual TexRPLib::IGPUContext* createContex() = 0;
	};

	// Enum gpu by index
	TEXRP_INTERFACE TexRPLib::IGPUInterface* enumGpuInterfaces(unsigned int index);
}
