#include "CommandList.h"

bool TexRP::GPUCommandList::init(ID3D12Device* ptrDevice, ID3D12CommandQueue* ptrCommandQueue, D3D12_COMMAND_LIST_TYPE type) {
	// Create command allocator
	if (FAILED(ptrDevice->CreateCommandAllocator(type, IID_PPV_ARGS(&m_ptrCommandAllocator)))) {
		return false;
	}

	// Create command list
	if (FAILED(ptrDevice->CreateCommandList(NULL, type, m_ptrCommandAllocator.Get(), NULL, IID_PPV_ARGS(&m_ptrCommandList)))) {
		return false;
	}
	
	// Passed
	return true;
}

void TexRP::GPUCommandList::release() {
	m_ptrCommandList.ReleaseAndGetAddressOf();
	m_ptrCommandAllocator.ReleaseAndGetAddressOf();
}

ID3D12GraphicsCommandList* TexRP::GPUCommandList::operator->() {
	return m_ptrCommandList.Get();
}

TexRP::GPUCommandList::operator ID3D12GraphicsCommandList* () {
	return m_ptrCommandList.Get();
}
