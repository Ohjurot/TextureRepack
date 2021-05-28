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
	// Create instance
	auto ptrContext = TexRPAllocate(IM_GPUContext, IM_GPUContext);
	
	// Init context
	if (!ptrContext->init(m_ptrAdapter.Get())) {
		// Failed delete and return null
		TexRPDestroy(ptrContext);
		return nullptr;
	}

	// Succeed return context pointer
	return ptrContext;
}

bool TexRPLib::IM_GPUInterface::setVRAMReservation(UINT64 numByte) {
	// Query for adpter 3
	ComPtr<IDXGIAdapter3> giAdpter3;
	if (SUCCEEDED(m_ptrAdapter.As(&giAdpter3))) {
		if (SUCCEEDED(giAdpter3->SetVideoMemoryReservation(NULL, (m_gpuDescription.DedicatedVideoMemory ? DXGI_MEMORY_SEGMENT_GROUP_LOCAL : DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL), numByte))) {
			return true;
		}
	}

	// Fallback
	return false;
}

UINT64 TexRPLib::IM_GPUInterface::getVRAMReservation() {
	// Reservation to be returned
	UINT64 reservation = 0;

	// Query for adpter 3
	ComPtr<IDXGIAdapter3> giAdpter3;
	if (SUCCEEDED(m_ptrAdapter.As(&giAdpter3))) {
		// Query for video memory info
		DXGI_QUERY_VIDEO_MEMORY_INFO memoryInfo;
		if (SUCCEEDED(giAdpter3->QueryVideoMemoryInfo(0, (m_gpuDescription.DedicatedVideoMemory ? DXGI_MEMORY_SEGMENT_GROUP_LOCAL : DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL), &memoryInfo))) {
			reservation = memoryInfo.CurrentReservation;
		}
	}

	// Return value
	return reservation;
}

IDXGIAdapter* TexRPLib::IM_GPUInterface::getAdpter() {
	return m_ptrAdapter.Get();
}
