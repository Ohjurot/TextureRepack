#pragma once

#include <TexRPLib/GPUInterface.h>

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

			// Adpter getter
			IDXGIAdapter* getAdpter();

		private:
			// GPU Adpater
			ComPtr<IDXGIAdapter> m_ptrAdapter = nullptr;

			// GPU Descriptor (LUID, name, etc ...)
			DXGI_ADAPTER_DESC m_gpuDescription;
	};
}
