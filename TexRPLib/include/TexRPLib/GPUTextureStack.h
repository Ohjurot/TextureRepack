#pragma once

#include <TexRPLib/TEXRP.h>
#include <TexRPLib/WinInclude.h>
#include <TexRPLib/GPUContext.h>

namespace TexRPLib {
	class IGPUTextureStack {
		public:
			// Virtual destructor
			virtual ~IGPUTextureStack() {};

			// Reset the texture stack to the defined parameters
			virtual bool reset(UINT width, UINT height, UINT bpp, UINT count) = 0;

			// Load a new texture from disk (return ID)
			virtual UINT loadFromDisk(LPCSTR path) = 0;

			// Create empty texture (width dummy path and reference texture for format)
			virtual UINT createEmpty(LPCSTR dummyPath, UINT reference) = 0;

			// Safe texture to disk
			virtual bool safeToDisk(UINT index) = 0;

			// Rename a texture
			virtual bool rename(UINT index, LPCSTR newName) = 0;
	};
}
