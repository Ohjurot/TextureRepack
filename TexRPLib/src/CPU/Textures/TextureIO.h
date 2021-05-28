#pragma once

#include <TexRPLib/WinInclude.h>
#include <CPU/Textures/TextureMeta.h>
#include <CPU/Textures/TextureTools.h>
#include <wincodec.h>

namespace TexRP {
	// Texture tools
	class TextureIO {
		public:
			// Reads the texture meta and source format
			static bool readTextureInformationFromFile(LPCSTR filePath, TEXTURE_META* ptrMeta, TEXTURE_FORMAT* ptrFormat);

			// Loads the texture from disk
			static bool loadTexture(const TEXTURE_META& metaInfo, const TEXTURE_FORMAT& sourceForamt, const TEXTURE_FORMAT& targetFormat, void* memoryPointer, size_t memorySize);
	};
}