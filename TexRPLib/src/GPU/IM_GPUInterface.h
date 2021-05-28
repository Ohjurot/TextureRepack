#pragma once

#include <TexRPLib/WinInclude.h>
#include <TexRPLib/GPUInterface.h>
#include <GPU/IM_GPUContext.h>

namespace TexRPLib {
	// Represents an interface to the gpu (IDXGIAdpter)
	class IM_GPUInterface : public IGPUInterface {
		public:
			// Constructor form GI adpter
			IM_GPUInterface(IDXGIAdapter* ptrAdpter);

			// Override interface functions
			const wchar_t* getGPUName() override;
			LUID getGPUUniqueId() override;
			UINT64 getGPUFreeVRAM() override;
			TexRPLib::IGPUContext* createContex() override;
			bool setVRAMReservation(UINT64 numByte) override;
			UINT64 getVRAMReservation() override;

			// Adpter getter
			IDXGIAdapter* getAdpter();

		private:
			// GPU Adpater
			ComPtr<IDXGIAdapter> m_ptrAdapter = nullptr;

			// GPU Descriptor (LUID, name, etc ...)
			DXGI_ADAPTER_DESC m_gpuDescription;
	};
}
