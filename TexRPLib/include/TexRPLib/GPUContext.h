#pragma once

#include <TexRPLib/WinInclude.h>
#include <TexRPLib/GPUTextureStack.h>
#include <TexRPLib/GPUGeometryModell.h>

namespace TexRPLib {
	class IGPUContext {
		public:
			// Virtual destructor
			virtual ~IGPUContext() {};

			// Check if the format is supported as input format (shader resource view)
			virtual bool checkInputSupport(DXGI_FORMAT format) = 0;

			// Check if the format is supported as output format (render target)
			virtual bool checkOutputSupport(DXGI_FORMAT format) = 0;

			// Create a gpu texture stack
			virtual TexRPLib::IGPUTextureStack* createTextureStack() = 0;

			// Opening a modell file from disk
			virtual TexRPLib::IGPUGeometryModell* openModell(LPCSTR modellPath) = 0;

			// Merg textures
			virtual bool mergTextures(TexRPLib::IGPUMask** arrMasks, TexRPLib::IGPUTextureStack* ptrTextureStack, UINT outputIndex, UINT mergCount, UINT* arrSourceIndices) = 0;
	};
}
