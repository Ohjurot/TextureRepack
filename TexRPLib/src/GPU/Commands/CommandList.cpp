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

void TexRP::GPUCommandList::close() {
	m_ptrCommandList->Close();
}

void TexRP::GPUCommandList::reset() {
	m_ptrCommandAllocator->Reset();
	m_ptrCommandList->Reset(m_ptrCommandAllocator.Get(), NULL);
}

ID3D12GraphicsCommandList* TexRP::GPUCommandList::ptr() {
	return m_ptrCommandList.Get();
}

ID3D12GraphicsCommandList* TexRP::GPUCommandList::operator->() {
	return m_ptrCommandList.Get();
}

TexRP::GPUCommandList::operator ID3D12GraphicsCommandList* () {
	return m_ptrCommandList.Get();
}
