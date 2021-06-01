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

bool TexRP::TextureIO::storeTexture(const TEXTURE_META& metaInfo, const TEXTURE_FORMAT& memoryFormat, const TEXTURE_FORMAT& fileFormat, void* memoryPointer, size_t memorySize) {
	// Succed indicator
	bool succeed = false;

	// Create WIC factory
	ComPtr<IWICImagingFactory> ptrWicFactory;
	if (SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&ptrWicFactory)))) {
		// Find GUID
		GUID encoderGUID;
		if (guidForFileEnding(metaInfo.fileName, &encoderGUID)) {
			// Create stream, encoder and temp meory buffers
			ComPtr<IWICBitmapEncoder> ptrFileEncoder;
			ComPtr<IWICStream> ptrOutStream;
			MallocdMemory<BYTE> streamTemp(memorySize);
			MallocdMemory<BYTE> convertedTemp(memorySize);
			if (
				streamTemp && convertedTemp &&
				SUCCEEDED(ptrWicFactory->CreateStream(&ptrOutStream)) &&
				SUCCEEDED(ptrWicFactory->CreateEncoder(encoderGUID, NULL, &ptrFileEncoder))
				) {
				// Init stream
				if (
					SUCCEEDED(ptrOutStream->InitializeFromMemory(streamTemp, memorySize)) &&
					SUCCEEDED(ptrFileEncoder->Initialize(ptrOutStream.Get(), WICBitmapEncoderNoCache))
					) {
					// Create frame encoder
					ComPtr<IWICBitmapFrameEncode> ptrFrameEncoder;
					if (SUCCEEDED(ptrFileEncoder->CreateNewFrame(&ptrFrameEncoder, NULL))) {
						// Output buffers
						BYTE* srcMemory = (BYTE*)memoryPointer;
						UINT srcSize = memorySize;

						// Bool indicating conversion issue
						bool continueImgSafe = true;

						// Check if conversion is required
						if (fileFormat.wicFileGuid != memoryFormat.wicFileGuid) {
							// Will go back to true if conversion is successfull
							continueImgSafe = false;

							// Create bitmap
							ComPtr<IWICBitmap> ptrBitmap;
							if (SUCCEEDED(ptrWicFactory->CreateBitmapFromMemory(metaInfo.width, metaInfo.height, memoryFormat.wicFileGuid, metaInfo.height * ((memoryFormat.bpp + 7) / 8), memorySize, (BYTE*)memoryPointer, &ptrBitmap))) {
								// Create format converter
								ComPtr<IWICFormatConverter> ptrFormatConverter;
								if (
									SUCCEEDED(ptrWicFactory->CreateFormatConverter(&ptrFormatConverter)) &&
									SUCCEEDED(ptrFormatConverter->Initialize(ptrBitmap.Get(), fileFormat.wicFileGuid, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut))
									) {
									// Copy pixels
									WICRect cpyRect = { 0, 0, metaInfo.width, metaInfo.height };
									if (SUCCEEDED(ptrFormatConverter->CopyPixels(&cpyRect, metaInfo.width * ((fileFormat.bpp + 7) / 8), convertedTemp.size(), convertedTemp))){
										// Set members
										srcMemory = convertedTemp;
										srcSize = metaInfo.height * metaInfo.width * ((fileFormat.bpp + 7) / 8);
										continueImgSafe = true;
									}	
								}
							}
						}
						
						// Init frame encoder
						if (continueImgSafe && SUCCEEDED(ptrFrameEncoder->Initialize(NULL))) {
							// Set encoder data
							WICPixelFormatGUID format = fileFormat.wicFileGuid;
							if (
								SUCCEEDED(ptrFrameEncoder->SetPixelFormat(&format)) &&
								SUCCEEDED(ptrFrameEncoder->SetSize(metaInfo.width, metaInfo.height)) &&
								SUCCEEDED(ptrFrameEncoder->WritePixels(metaInfo.height, metaInfo.width * ((fileFormat.bpp + 7) / 8), srcSize, srcMemory))
								) {
								// Commit streams
								if (
									SUCCEEDED(ptrFrameEncoder->Commit()) &&
									SUCCEEDED(ptrFileEncoder->Commit())
									) {
									// Get length of output file (stream)
									ULARGE_INTEGER length;
									if (SUCCEEDED(ptrOutStream->Seek({ 0,0 }, STREAM_SEEK_CUR, &length))) {
										// Open output image
										HANDLE hFile = CreateFileA(metaInfo.fileName, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
										if (hFile != INVALID_HANDLE_VALUE) {
											// Write to image
											if (WriteFile(hFile, streamTemp, length.LowPart, NULL, NULL) == TRUE) {
												// Write succeeded
												succeed = true;
											}

											// Close handle
											CloseHandle(hFile);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// Return succeed
	return succeed;
}

bool TexRP::TextureIO::guidForFileEnding(LPCSTR filePath, GUID* ptrGUID) {
	// Find file ending
	LPCSTR ending = strrchr(filePath, '.');
	if (ending) {
		ending++;

		// GUID_ContainerFormatBmp
		if (strcmp(ending, "bmp") == 0) {
			*ptrGUID = GUID_ContainerFormatBmp;
			return true;
		}
		// GUID_ContainerFormatPng
		else if (strcmp(ending, "png") == 0) {
			*ptrGUID = GUID_ContainerFormatPng;
			return true;
		}
		// GUID_ContainerFormatIco
		else if (strcmp(ending, "ico") == 0) {
			*ptrGUID = GUID_ContainerFormatIco;
			return true;
		}
		// GUID_ContainerFormatJpeg
		else if (strcmp(ending, "jpeg") == 0) {
			*ptrGUID = GUID_ContainerFormatJpeg;
			return true;
		}
		// GUID_ContainerFormatTiff
		else if (strcmp(ending, "tiff") == 0) {
			*ptrGUID = GUID_ContainerFormatTiff;
			return true;
		}
		// GUID_ContainerFormatGif
		else if (strcmp(ending, "gif") == 0) {
			*ptrGUID = GUID_ContainerFormatGif;
			return true;
		}
		// GUID_ContainerFormatWmp
		else if (strcmp(ending, "wmp") == 0) {
			*ptrGUID = GUID_ContainerFormatWmp;
			return true;
		}
	}
	
	// Fallback
	return false;
}
