#include "IM_GPUMask.h"

TexRPLib::IM_GPUMask::~IM_GPUMask() {
	m_ptrMaskResource.ReleaseAndGetAddressOf();
}

bool TexRPLib::IM_GPUMask::init(ID3D12Device* ptrDevice, UINT sideLength) {
	// Set side length
	m_sideLength = sideLength;
	
	// Describe heap
	D3D12_HEAP_PROPERTIES hp;
	ZeroMemory(&hp, sizeof(D3D12_HEAP_PROPERTIES));
	hp.Type = D3D12_HEAP_TYPE_DEFAULT;
	hp.CreationNodeMask = NULL;
	hp.VisibleNodeMask = NULL;

	// Describe resource
	D3D12_RESOURCE_DESC rd;
	ZeroMemory(&rd, sizeof(D3D12_RESOURCE_DESC));
	rd.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	rd.Alignment = 64 * 1024;
	rd.Width = sideLength;
	rd.Height = sideLength;
	rd.DepthOrArraySize = 1;
	rd.MipLevels = 1;
	rd.Format = DXGI_FORMAT_R8_UNORM;
	rd.SampleDesc.Count = 1;
	rd.SampleDesc.Quality = 0;
	rd.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	rd.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	// Describe clear value
	D3D12_CLEAR_VALUE clearValue;
	ZeroMemory(&clearValue, sizeof(D3D12_CLEAR_VALUE));
	clearValue.Format = DXGI_FORMAT_R8_UNORM;
	clearValue.Color[0] = 0.0f;
	clearValue.Color[1] = 0.0f;
	clearValue.Color[2] = 0.0f;
	clearValue.Color[3] = 0.0f;

	// Create resource
	if (FAILED(ptrDevice->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd, D3D12_RESOURCE_STATE_RENDER_TARGET, &clearValue, IID_PPV_ARGS(&m_ptrMaskResource)))) {
		return false;
	}
	m_currentState = D3D12_RESOURCE_STATE_RENDER_TARGET;

	// Describe descritor heap
	D3D12_DESCRIPTOR_HEAP_DESC rtvDhd;
	ZeroMemory(&rtvDhd, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	rtvDhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDhd.NumDescriptors = 1;
	rtvDhd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDhd.NodeMask = NULL;

	// Create descritor heap
	if (FAILED(ptrDevice->CreateDescriptorHeap(&rtvDhd, IID_PPV_ARGS(&m_ptrRTVHeap)))) {
		return false;
	}

	// Create RTV
	ptrDevice->CreateRenderTargetView(m_ptrMaskResource.Get(), NULL, m_ptrRTVHeap->GetCPUDescriptorHandleForHeapStart());

	// Passed
	return true;
}

void TexRPLib::IM_GPUMask::bindAsRTV(ID3D12GraphicsCommandList* ptrCommandList) {
	// Check for state change
	if (m_currentState != D3D12_RESOURCE_STATE_RENDER_TARGET) {
		// Resource barrier
		D3D12_RESOURCE_BARRIER barr;
		ZeroMemory(&barr, sizeof(D3D12_RESOURCE_BARRIER));
		barr.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barr.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barr.Transition.pResource = m_ptrMaskResource.Get();
		barr.Transition.Subresource = NULL;
		barr.Transition.StateBefore = m_currentState;
		barr.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		// Queue barrier
		ptrCommandList->ResourceBarrier(1, &barr);

		// Update state
		m_currentState = D3D12_RESOURCE_STATE_RENDER_TARGET;
	}

	// RTV Handle
	auto rtvHandle = m_ptrRTVHeap->GetCPUDescriptorHandleForHeapStart();

	// Clear rtv
	if (m_shouldClear) {
		// Clear rtv
		static const float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		ptrCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, NULL);

		// Clear flag
		m_shouldClear = false;
	}

	// Set RTV
	ptrCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, NULL);

	// Set viewport
	const D3D12_VIEWPORT vp = { 0, 0, m_sideLength, m_sideLength, 0.0f, 1.0f };
	ptrCommandList->RSSetViewports(1, &vp);

	// Set siccor rect
	const D3D12_RECT sr = {0, 0, m_sideLength, m_sideLength };
	ptrCommandList->RSSetScissorRects(1, &sr);
}

void TexRPLib::IM_GPUMask::clear() {
	m_shouldClear = true;
}
