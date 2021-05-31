#pragma once

#include <TexRPLib/WinInclude.h>

#include <iostream>

#ifdef _DEBUG
static ComPtr<ID3D12Debug> s_debugInterface;
#endif

BOOL WINAPI DllMain(HINSTANCE dllInstance, DWORD callReason, LPVOID reserved) {
	// Switch on call reason
	switch (callReason) {
		// Lib loaded
		case DLL_PROCESS_ATTACH: {
			// CoInit
			switch(CoInitializeEx(NULL, COINIT_MULTITHREADED)) {
				// Means coinit was successfull
				case S_OK:
					[[fallthrough]];
				// Mans CoInit was already called
				case S_FALSE:
					// Will both pass
					break;
				
				// Any other error code will prevent the dll from loading
				default:
					return FALSE;
			}

			// DirectX debug
			#ifdef _DEBUG
			if(FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&s_debugInterface)))){
				return FALSE;
			}
			s_debugInterface->EnableDebugLayer();
			#endif

			break;
		}

		// Lib unload
		case DLL_PROCESS_DETACH: {
			// DirectX debug
			#ifdef _DEBUG
			s_debugInterface.ReleaseAndGetAddressOf();
			#endif			
			break;
		}

		// Thread attached
		case DLL_THREAD_ATTACH: {
			// CoInit
			switch(CoInitializeEx(NULL, COINIT_MULTITHREADED)) {
				// Means coinit was successfull
				case S_OK:
					[[fallthrough]];
				// Mans CoInit was already called
				case S_FALSE:
					// Will both pass
					break;
				
				// Any other mens error
				default:
					return FALSE;
			}
			break;
		} 
	}

	// Load passed
	return TRUE;
}
