#pragma once

#include <TexRPLib/WinInclude.h>

namespace TexRP {
	// Texture meta information
	struct TEXTURE_META {
		// Name of file
		LPCSTR fileName;

		// Width of the texture
		UINT width;

		// Height of the texture
		UINT height;
	};

	// Texture format information
	struct TEXTURE_FORMAT {
		// Wic GUID representing the file
		GUID wicFileGuid;

		// DirectX format representing the texture
		DXGI_FORMAT d3dFormat;

		// Format bpp
		UINT bpp;
	};

	// Info about a texture
	struct TEXTURE_INFO {
		// Texture meta information
		TEXTURE_META metaInfo;

		// Format on disk
		TEXTURE_FORMAT sourceFormat;
		
		// Format on GPU
		TEXTURE_FORMAT targetFormat;
	};
}
