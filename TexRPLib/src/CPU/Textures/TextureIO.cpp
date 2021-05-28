#include "TextureIO.h"

bool TexRP::TextureIO::readTextureInformationFromFile(LPCSTR filePath, TEXTURE_META* ptrMeta, TEXTURE_FORMAT* ptrFormat) {
	// Suceed flag 
	bool succeed = false;
	
	// Open file handle
	HANDLE hFile = CreateFileA(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		// Store filename
		ptrMeta->fileName = filePath;

		// Create WIC factory
		ComPtr<IWICImagingFactory> ptrWicFactory;
		if (SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&ptrWicFactory)))) {
			// Create decoder from file handle
			ComPtr<IWICBitmapDecoder> ptrDecoder;
			if (SUCCEEDED(ptrWicFactory->CreateDecoderFromFileHandle((ULONG_PTR)hFile, NULL, WICDecodeMetadataCacheOnDemand, &ptrDecoder))) {
				// Get first (and only) frame form image
				ComPtr<IWICBitmapFrameDecode> ptrFrameDecoder;
				if (SUCCEEDED(ptrDecoder->GetFrame(0, &ptrFrameDecoder))) {
					// Get pixel format from frame decoder and size of image
					if (SUCCEEDED(ptrFrameDecoder->GetPixelFormat(&ptrFormat->wicFileGuid)) && SUCCEEDED(ptrFrameDecoder->GetSize(&ptrMeta->width, &ptrMeta->height))) {
						// Try Map a directx format
						ptrFormat->d3dFormat = TextureTools::dxgiFormatForGUID(ptrFormat->wicFileGuid);

						// Retrive GUID info
						ComPtr<IWICComponentInfo> ptrComponentInfo;
						if (SUCCEEDED(ptrWicFactory->CreateComponentInfo(ptrFormat->wicFileGuid, &ptrComponentInfo))) {
							// Convert to IWICPixelFormatInfo
							ComPtr<IWICPixelFormatInfo> ptrFormatInfo;
							if (SUCCEEDED(ptrComponentInfo->QueryInterface(IID_PPV_ARGS(&ptrFormatInfo)))) {
								// Get bpp
								if (SUCCEEDED(ptrFormatInfo->GetBitsPerPixel(&ptrFormat->bpp))) {
									succeed = true;
								}
							}
						}
					}
				}
			}
		}

		// Close file handle
		CloseHandle(hFile);
	}
	
	// Return resulting flag
	return succeed;
}

bool TexRP::TextureIO::loadTexture(const TEXTURE_META& metaInfo, const TEXTURE_FORMAT& sourceFormat, const TEXTURE_FORMAT& targetFormat, void* memoryPointer, size_t memorySize) {
	// Check buffer size
	if (memorySize < ((targetFormat.bpp + 7) / 8) * metaInfo.width * metaInfo.height) {
		return false;
	}

	// Succeed flag
	bool succeed = false;

	// Describe wic copy area
	WICRect copyRect;
	copyRect.X = 0;
	copyRect.Y = 0;
	copyRect.Width = metaInfo.width;
	copyRect.Height = metaInfo.height;

	// Open file handle
	HANDLE hFile = CreateFileA(metaInfo.fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		// Create WIC factory
		ComPtr<IWICImagingFactory> ptrWicFactory;
		if (SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&ptrWicFactory)))) {
			// Create decoder from file handle
			ComPtr<IWICBitmapDecoder> ptrDecoder;
			if (SUCCEEDED(ptrWicFactory->CreateDecoderFromFileHandle((ULONG_PTR)hFile, NULL, WICDecodeMetadataCacheOnDemand, &ptrDecoder))) {
				// Get first (and only) frame form image
				ComPtr<IWICBitmapFrameDecode> ptrFrameDecoder;
				if (SUCCEEDED(ptrDecoder->GetFrame(0, &ptrFrameDecoder))) {
					// Check if conversion if direct copy is possible
					if (memcmp(&targetFormat.wicFileGuid, &sourceFormat.wicFileGuid, sizeof(GUID)) == 0) {
						// Copy pixels
						if (SUCCEEDED(ptrFrameDecoder->CopyPixels(&copyRect, ((targetFormat.bpp + 7) / 8) * metaInfo.width, memorySize, (BYTE*)memoryPointer))) {
							succeed = true;
						}
					}
					// Need conversion
					else {
						// Create a converter
						ComPtr<IWICFormatConverter> wicConverter;
						if (SUCCEEDED(ptrWicFactory->CreateFormatConverter(&wicConverter))) {
							// Check if conversion is possible
							BOOL canConvert = FALSE;
							if (SUCCEEDED(wicConverter->CanConvert(sourceFormat.wicFileGuid, targetFormat.wicFileGuid, &canConvert)) && canConvert) {
								// Init converter
								if (SUCCEEDED(wicConverter->Initialize(ptrFrameDecoder.Get(), targetFormat.wicFileGuid, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut))) {
									// Copy pixels
									if (SUCCEEDED(wicConverter->CopyPixels(&copyRect, ((targetFormat.bpp + 7) / 8) * metaInfo.width, memorySize, (BYTE*)memoryPointer))) {
										succeed = true;
									}
								}
							}
						}
					}
				}
			}
		}

		// Close file handle
		CloseHandle(hFile);
	}

	// Return flaged result
	return succeed;
}
