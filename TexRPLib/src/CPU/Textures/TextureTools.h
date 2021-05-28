#pragma once

#include <TexRPLib/WinInclude.h>
#include <CPU/Textures/TextureMeta.h>
#include <wincodec.h>

namespace TexRP {
	// Tools that help with texturing
	class TextureTools {
		public:
			// Converts a wic format to a directx format (if possible)
			static DXGI_FORMAT dxgiFormatForGUID(GUID wicFormat);

			// Finds a matching format
			static bool matchFormat(const TEXTURE_FORMAT& inputFormat, TEXTURE_FORMAT* ptrOutputFormat);

		private:
			// GUID to GUID type
			struct GUID_GUID_T {
				GUID inputFormat;
				TEXTURE_FORMAT outputFormat;
			};

			// Conversion table for GUID to DXGI
			static TEXTURE_FORMAT s_conversionTable_GUID_DXIG[];
			
			// Conversion table for GUID to GUID / DXGI
			static GUID_GUID_T s_conversionTable_GUID_GUID[];
	};
}