#include "IM_GPUContext.h"

TexRPLib::IM_GPUContext::~IM_GPUContext() {
	// Release pass
	m_maskPass.release();
	m_mergePass.release();
	
	// Release mebers
	m_directCommandList.release();
	m_directCommandQueue.release();
	m_ptrDevice.ReleaseAndGetAddressOf();

	#ifdef _DEBUG
	if (m_pixCaptureControle) {
		m_pixCaptureControle->EndCapture();
		m_pixCaptureControle.ReleaseAndGetAddressOf();
	}

	if (m_ptrDebugDevice) {
		OutputDebugStringW(L"--- DirectX 12 is reporting live device objects (final destruction). Please note that one device reported is totaly normal!\n--- This device listed it that device that is producing that output\n");
		m_ptrDebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_IGNORE_INTERNAL | D3D12_RLDO_DETAIL);
	}
	#endif
}

bool TexRPLib::IM_GPUContext::init(IDXGIAdapter* ptrAdpter) {
	// Create device
	if (FAILED(D3D12CreateDevice(ptrAdpter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_ptrDevice)))) {
		return false;
	}

	// Create debug device
	#ifdef _DEBUG
	if (FAILED(m_ptrDevice->QueryInterface(IID_PPV_ARGS(&m_ptrDebugDevice)))) {
		return false;
	}

	// PIX
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&m_pixCaptureControle)))) {
		m_pixCaptureControle->BeginCapture();
	}
	#endif

	// Init command queue
	if (!m_directCommandQueue.init(m_ptrDevice.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT)) {
		return false;
	}

	// Init command list
	if (!m_directCommandList.init(m_ptrDevice.Get(), m_directCommandQueue, D3D12_COMMAND_LIST_TYPE_DIRECT)) {
		return false;
	}

	// Check for conservative rasterizer	
	D3D12_FEATURE_DATA_D3D12_OPTIONS options;
	ZeroMemory(&options, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS));
	if (FAILED(m_ptrDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS))) || options.ConservativeRasterizationTier == D3D12_CONSERVATIVE_RASTERIZATION_TIER_NOT_SUPPORTED) {
		return false;
	}

	// Init mask pass
	if (!m_maskPass.init(m_ptrDevice.Get())) {
		return false;
	}

	// Init merge pass
	if (!m_mergePass.init(m_ptrDevice.Get(), &m_directCommandQueue, &m_directCommandList)) {
		return false;
	}

	// Passed
	return true;
}

bool TexRPLib::IM_GPUContext::checkInputSupport(DXGI_FORMAT format) {
	// Fill query information
	D3D12_FEATURE_DATA_FORMAT_SUPPORT formatInfo;
	formatInfo.Format = format;
	
	// Query for information
	if (SUCCEEDED(m_ptrDevice->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatInfo, sizeof(D3D12_FEATURE_DATA_FORMAT_SUPPORT)))) {
		return formatInfo.Support1 & D3D12_FORMAT_SUPPORT1_TEXTURE2D;
	}

	// Fallback
	return false;
}

bool TexRPLib::IM_GPUContext::checkOutputSupport(DXGI_FORMAT format) {
	// Fill query information
	D3D12_FEATURE_DATA_FORMAT_SUPPORT formatInfo;
	formatInfo.Format = format;

	// Query for information
	if (SUCCEEDED(m_ptrDevice->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatInfo, sizeof(D3D12_FEATURE_DATA_FORMAT_SUPPORT)))) {
		return formatInfo.Support1 & D3D12_FORMAT_SUPPORT1_RENDER_TARGET;
	}

	// Fallback
	return false;
}

TexRPLib::IGPUTextureStack* TexRPLib::IM_GPUContext::createTextureStack() {
	// Allocate texture stack
	TexRPLib::IM_GPUTextureStack* ptrGpuTextureStack = TexRPAllocate(IM_GPUTextureStack, IM_GPUTextureStack);
	if (ptrGpuTextureStack->init(m_ptrDevice.Get(), &m_directCommandQueue, &m_directCommandList)) {
		return ptrGpuTextureStack;
	}

	// Deallocate
	TexRPDestroy(ptrGpuTextureStack);
	return nullptr;
}

TexRPLib::IGPUGeometryModell* TexRPLib::IM_GPUContext::openModell(LPCSTR modellPath) {
	// Allocate texture stack
	TexRPLib::IM_GPUGeometryModell* ptrGpuModell = TexRPAllocate(IM_GPUGeometryModell, IM_GPUGeometryModell);
	if (ptrGpuModell->init(m_ptrDevice.Get(), &m_directCommandList, &m_directCommandQueue, modellPath, &m_maskPass)) {
		return ptrGpuModell;
	}

	// Deallocate
	TexRPDestroy(ptrGpuModell);
	return nullptr;
}

bool TexRPLib::IM_GPUContext::mergTextures(TexRPLib::IGPUMask** arrMasks, TexRPLib::IGPUTextureStack* ptrTextureStack, UINT outputIndex, UINT mergCount, UINT* arrSourceIndices) {
	// Cast to implementation pointers
	IM_GPUTextureStack* ptrTextureStackIM = (IM_GPUTextureStack*)ptrTextureStack;

	// TODO: Validate

	// Describe descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC dhd;
	ZeroMemory(&dhd, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	dhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	dhd.NumDescriptors = mergCount * 2;
	dhd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	dhd.NodeMask = NULL;

	// create descriptor heap
	ComPtr<ID3D12DescriptorHeap> srvHeap;
	if (FAILED(m_ptrDevice->CreateDescriptorHeap(&dhd, IID_PPV_ARGS(&srvHeap)))) {
		return false;
	}

	// Heap increment
	auto heapIncrmentSize = m_ptrDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Bind heap
	ID3D12DescriptorHeap* heaps[] = {srvHeap.Get()};
	m_directCommandList->SetDescriptorHeaps(1, heaps);

	// Bind state
	m_mergePass.bind(m_directCommandList);
	ptrTextureStackIM->setRenderTarget(outputIndex, m_directCommandList);
	
	// Prepare texture srvs
	ptrTextureStackIM->setShaderResourceViews(arrSourceIndices, mergCount, srvHeap->GetCPUDescriptorHandleForHeapStart());
	
	// Prepare mask srvs
	for (UINT i = 0; i < mergCount; i++) {
		// Get handle
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = { srvHeap->GetCPUDescriptorHandleForHeapStart().ptr + ((mergCount + i) * heapIncrmentSize) };

		// Create SRV
		m_ptrDevice->CreateShaderResourceView(((IM_GPUMask*)arrMasks[i])->res(), NULL, srvHandle);
	}
	
	// SRV Binding
	m_directCommandList->SetGraphicsRootDescriptorTable(1, srvHeap->GetGPUDescriptorHandleForHeapStart());
	m_directCommandList->SetGraphicsRootDescriptorTable(0, { srvHeap->GetGPUDescriptorHandleForHeapStart().ptr + (mergCount * heapIncrmentSize)});

	// Draw
	m_directCommandList->DrawInstanced(4, mergCount, 0, 0);

	// Exeute
	m_directCommandList.close();
	ID3D12CommandList* executeArray[] = { m_directCommandList };
	m_directCommandQueue.dispatchSync(executeArray, 1);
	m_directCommandList.reset();

	// Pass
	return true;
}
