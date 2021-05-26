#include "IM_GPUInterface.h"

TexRPLib::IM_GPUInterface::IM_GPUInterface(IDXGIAdapter* ptrAdpter) : 
	m_ptrAdapter(ptrAdpter)
{
	// Get adpter info
	ptrAdpter->GetDesc(&m_gpuDescription);
}

const wchar_t* TexRPLib::IM_GPUInterface::getGPUName() {
	return m_gpuDescription.Description;
}

LUID TexRPLib::IM_GPUInterface::getGPUUniqueId() {
	return m_gpuDescription.AdapterLuid;
}

UINT64 TexRPLib::IM_GPUInterface::getGPUFreeVRAM() {
	// VRam to be returned
	UINT64 freeVRam = 0;

	// Query for adpter 3
	ComPtr<IDXGIAdapter3> giAdpter3;
	if (SUCCEEDED(m_ptrAdapter.As(&giAdpter3))) {
		// Query for video memory info
		DXGI_QUERY_VIDEO_MEMORY_INFO memoryInfo;
		if (SUCCEEDED(giAdpter3->QueryVideoMemoryInfo(0, (m_gpuDescription.DedicatedVideoMemory ? DXGI_MEMORY_SEGMENT_GROUP_LOCAL : DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL), &memoryInfo))) {
			freeVRam = memoryInfo.Budget;
		}
	}

	// Return value
	return freeVRam;
}

TexRPLib::IGPUContext* TexRPLib::IM_GPUInterface::createContex() {
	// TODO: Real implementation
	return TexRPAllocate(IGPUContext, IGPUContext);
}

IDXGIAdapter* TexRPLib::IM_GPUInterface::getAdpter() {
	return m_ptrAdapter.Get();
}
