#include "TextureTools.h"

TexRP::TEXTURE_FORMAT TexRP::TextureTools::s_conversionTable_GUID_DXIG[] = {
	// 32 BPP
	{GUID_WICPixelFormat32bppRGBA, DXGI_FORMAT_R8G8B8A8_UNORM, 32},
	{GUID_WICPixelFormat32bppRGB, DXGI_FORMAT_R8G8B8A8_UNORM, 32},
	// 64 BPP
	{GUID_WICPixelFormat64bppRGBA, DXGI_FORMAT_R16G16B16A16_UNORM, 64},
	{GUID_WICPixelFormat64bppRGB, DXGI_FORMAT_R16G16B16A16_UNORM, 64}
};

TexRP::TextureTools::GUID_GUID_T TexRP::TextureTools::s_conversionTable_GUID_GUID[]{
	// Grayscale input (1 CH) 8-Bit
	{GUID_WICPixelFormat8bppGray, {GUID_WICPixelFormat32bppRGBA, DXGI_FORMAT_R8G8B8A8_UNORM, 32}},
	// Grayscale input (1 CH) 16-Bit
	{GUID_WICPixelFormat16bppGray, {GUID_WICPixelFormat64bppRGBA, DXGI_FORMAT_R16G16B16A16_UNORM, 64}},
	// No alpha input (3 CH) 8-Bit
	{GUID_WICPixelFormat24bppRGB, {GUID_WICPixelFormat32bppRGBA, DXGI_FORMAT_R8G8B8A8_UNORM, 32} },
	{GUID_WICPixelFormat24bppBGR, {GUID_WICPixelFormat32bppRGBA, DXGI_FORMAT_R8G8B8A8_UNORM, 32} },
	{GUID_WICPixelFormat32bppBGR, {GUID_WICPixelFormat32bppRGBA, DXGI_FORMAT_R8G8B8A8_UNORM, 32} },
	// No alpha input (3 CH) 16-Bit
	{GUID_WICPixelFormat48bppRGB, {GUID_WICPixelFormat64bppRGBA, DXGI_FORMAT_R16G16B16A16_UNORM, 64}},
	{GUID_WICPixelFormat48bppBGR, {GUID_WICPixelFormat64bppRGBA, DXGI_FORMAT_R16G16B16A16_UNORM, 64}},
	// Fliped (4 CH) 8-Bit
	{GUID_WICPixelFormat32bppBGRA, {GUID_WICPixelFormat32bppRGBA, DXGI_FORMAT_R8G8B8A8_UNORM, 32} },
	// Fliped (4 CH) 16-Bit
	{GUID_WICPixelFormat64bppBGRA, {GUID_WICPixelFormat64bppRGBA, DXGI_FORMAT_R16G16B16A16_UNORM, 64}}
};

DXGI_FORMAT TexRP::TextureTools::dxgiFormatForGUID(GUID wicFormat) {
	// Find GUID
	for (unsigned int i = 0; i < _countof(s_conversionTable_GUID_DXIG); i++) {
		if (memcmp(&wicFormat, &s_conversionTable_GUID_DXIG[i].wicFileGuid, sizeof(GUID)) == 0) {
			return s_conversionTable_GUID_DXIG[i].d3dFormat;
		}
	}
	
	// Fallback unknow format
	return DXGI_FORMAT_UNKNOWN;
}

bool TexRP::TextureTools::matchFormat(const TEXTURE_FORMAT& inputFormat, TEXTURE_FORMAT* ptrOutputFormat) {
	// Check if match is shortcut is availible
	if (inputFormat.d3dFormat != DXGI_FORMAT_UNKNOWN) {
		// Copy input
		memcpy(ptrOutputFormat, &inputFormat, sizeof(TEXTURE_FORMAT));
		return true;
	}
	// Find matching
	else {
		for (unsigned int i = 0; i < _countof(s_conversionTable_GUID_GUID); i++) {
			if (memcmp(&inputFormat.wicFileGuid, &s_conversionTable_GUID_GUID[i].inputFormat, sizeof(GUID)) == 0) {
				// Copy and return
				memcpy(ptrOutputFormat, &s_conversionTable_GUID_GUID[i].outputFormat, sizeof(TEXTURE_FORMAT));
				return true;
			}
		}
	}
	
	// Fallback false
	return false;
}
