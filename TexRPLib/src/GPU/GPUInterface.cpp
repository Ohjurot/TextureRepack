#include <TexRPLib/GPUInterface.h>
#include <GPU/IM_GPUInterface.h>

TEXRP_INTERFACE TexRPLib::IGPUInterface* TexRPLib::enumGpuInterfaces(unsigned int index) {
	// Retrive GI Factory
	ComPtr<IDXGIFactory> giFactory;
	if (SUCCEEDED(CreateDXGIFactory(IID_PPV_ARGS(&giFactory)))) {
		ComPtr<IDXGIAdapter> giAdapter;
		if (giFactory->EnumAdapters(index, &giAdapter) == S_OK) {
			// TODO: Create GPUInterface
			return TexRPAllocate(IGPUInterface, IM_GPUInterface, giAdapter.Get());
		}
	}
	
	// Fallback nullpointer
	return nullptr;
}
