#include "IM_GPUContext.h"

TexRPLib::IM_GPUContext::~IM_GPUContext() {
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

	// DEBUG VERY VERY DIRTY TO DO THAT HERE BUT DLL DOES NOT EXPOSE ALL REQUIRED CLASSES TO TEST THIS DURING DEVELOPMENT

	// END DEBUG

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
