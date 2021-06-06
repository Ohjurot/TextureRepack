#include "MergeRenderPass.h"

TexRP::Passes::MergeRenderPass::~MergeRenderPass() {
	release();
}

bool TexRP::Passes::MergeRenderPass::init(ID3D12Device* ptrDevice, TexRP::GPUCommandQueue* ptrCommandQueue, TexRP::GPUCommandList* ptrCommandList) {
	// Get path to module
	CHAR moduelPath[1024];
	DWORD mfnRes = GetModuleFileNameA(GetModuleHandle(NULL), moduelPath, 1024);
	if (mfnRes == 0 || mfnRes == ERROR_INSUFFICIENT_BUFFER) {
		return false;
	}

	// Remove name and .exe
	CHAR* slashPointer = strrchr(moduelPath, '\\');
	slashPointer[1] = '\0';

	// Load shaders
	CHAR shaderPath[1024];

	// Load root signature
	shaderPath[0] = '\0';
	strcpy_s<1024>(shaderPath, moduelPath);
	strcat_s<1024>(shaderPath, "MergePass_RS.cso");
	m_rootSignature = GPUShader(shaderPath);

	// Load vertex shader
	shaderPath[0] = '\0';
	strcpy_s<1024>(shaderPath, moduelPath);
	strcat_s<1024>(shaderPath, "MergePass_VS.cso");
	m_vertexShader = GPUShader(shaderPath);

	// Load pixel shader
	shaderPath[0] = '\0';
	strcpy_s<1024>(shaderPath, moduelPath);
	strcat_s<1024>(shaderPath, "MergePass_PS.cso");
	m_pixelShader = GPUShader(shaderPath);

	// Check shader
	if (!m_rootSignature || !m_vertexShader || !m_pixelShader) {
		return false;
	}

	// Create root signature
	if (FAILED(ptrDevice->CreateRootSignature(NULL, m_rootSignature.ptr(), m_rootSignature.size(), IID_PPV_ARGS(&m_ptrRootSignature)))) {
		return false;
	}

	// Build input layout
	static const D3D12_INPUT_ELEMENT_DESC sc_inputLayout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	// Create PSO Descriptor
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	// Describe PSO: RS & Shader
	psoDesc.pRootSignature = m_ptrRootSignature.Get();
	psoDesc.VS = m_vertexShader.bytecodeDesc();
	psoDesc.PS = m_pixelShader.bytecodeDesc();

	// Describe PSO: Stram out
	psoDesc.StreamOutput.NumEntries = 0;
	psoDesc.StreamOutput.NumStrides = 0;

	// Describe PSO: Blend State
	psoDesc.BlendState.AlphaToCoverageEnable = FALSE;
	psoDesc.BlendState.IndependentBlendEnable = FALSE;
	psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
	psoDesc.BlendState.RenderTarget[0].LogicOpEnable = FALSE;
	psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	psoDesc.SampleMask = 0xFF;

	// Describe PSO: Rasterizer
	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	psoDesc.RasterizerState.FrontCounterClockwise = FALSE;
	psoDesc.RasterizerState.DepthBias = 0;
	psoDesc.RasterizerState.DepthBiasClamp = 0.0f;
	psoDesc.RasterizerState.SlopeScaledDepthBias = 0.0f;
	psoDesc.RasterizerState.DepthClipEnable = FALSE;
	psoDesc.RasterizerState.MultisampleEnable = FALSE;
	psoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
	psoDesc.RasterizerState.ForcedSampleCount = 1;
	psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	// Describe PSO: Depth
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;

	// Describe PSO: Input layout
	psoDesc.InputLayout.NumElements = _countof(sc_inputLayout);
	psoDesc.InputLayout.pInputElementDescs = sc_inputLayout;
	psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// Describe PSO: Render target
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;

	// Describe PSO: Meta
	psoDesc.NodeMask = NULL;
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	// Create PSO
	if (FAILED(ptrDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_ptrPSO)))) {
		return false;
	}

	// Upload heap propertie
	D3D12_HEAP_PROPERTIES propUpload;
	ZeroMemory(&propUpload, sizeof(D3D12_HEAP_PROPERTIES));
	propUpload.Type = D3D12_HEAP_TYPE_UPLOAD;
	propUpload.CreationNodeMask = NULL;
	propUpload.VisibleNodeMask = NULL;

	// Download heap propertie
	D3D12_HEAP_PROPERTIES propDefault;
	ZeroMemory(&propDefault, sizeof(D3D12_HEAP_PROPERTIES));
	propDefault.Type = D3D12_HEAP_TYPE_DEFAULT;
	propDefault.CreationNodeMask = NULL;
	propDefault.VisibleNodeMask = NULL;

	// Describe vertex buffer
	D3D12_RESOURCE_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D12_RESOURCE_DESC));
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Alignment = 1024 * 64;
	bufferDesc.Width = sizeof(VertexForPass) * 4;
	bufferDesc.Height = 1;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.MipLevels = 1;
	bufferDesc.SampleDesc.Count = 1;
	bufferDesc.SampleDesc.Quality = 0;
	bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// Create vertex buffer
	if (FAILED(ptrDevice->CreateCommittedResource(&propDefault, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_COPY_DEST, NULL, IID_PPV_ARGS(&m_ptrVertexBuffer)))) {
		return false;
	}

	// Upload buffer
	ComPtr<ID3D12Resource> uploadBuffer;
	if (FAILED(ptrDevice->CreateCommittedResource(&propUpload, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&uploadBuffer)))) {
		return false;
	}

	// Map uplod buffer	
	void* vertexBufferCpu;
	if (FAILED(uploadBuffer->Map(NULL, NULL, &vertexBufferCpu))) {
		return false;
	}

	// Copy to upload buffer
	memcpy(vertexBufferCpu, c_verticies, sizeof(VertexForPass) * 4);

	// Unmap
	uploadBuffer->Unmap(NULL, NULL);

	// Copy on the GPU
	ptrCommandList->ptr()->CopyBufferRegion(m_ptrVertexBuffer.Get(), 0, uploadBuffer.Get(), 0, sizeof(VertexForPass) * 4);
	
	// Resource barrier to vertex buffer
	D3D12_RESOURCE_BARRIER barr;
	ZeroMemory(&barr, sizeof(D3D12_RESOURCE_BARRIER));
	barr.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barr.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barr.Transition.pResource = m_ptrVertexBuffer.Get();
	barr.Transition.Subresource = NULL;
	barr.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barr.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	ptrCommandList->ptr()->ResourceBarrier(1, &barr);

	// Execute command list
	ptrCommandList->close();
	ID3D12CommandList* executeArray[] = { *ptrCommandList };
	ptrCommandQueue->dispatchSync(executeArray, 1);
	ptrCommandList->reset();

	// Pass
	return true;
}

void TexRP::Passes::MergeRenderPass::release() {
	m_ptrVertexBuffer.ReleaseAndGetAddressOf();
	m_ptrPSO.ReleaseAndGetAddressOf();
	m_ptrRootSignature.ReleaseAndGetAddressOf();
}

void TexRP::Passes::MergeRenderPass::bind(ID3D12GraphicsCommandList* ptrCommandList) {
	// Set state and root
	ptrCommandList->SetPipelineState(m_ptrPSO.Get());
	ptrCommandList->SetGraphicsRootSignature(m_ptrRootSignature.Get());
	
	// Describe the view
	D3D12_VERTEX_BUFFER_VIEW vbView;
	ZeroMemory(&vbView, sizeof(D3D12_VERTEX_BUFFER_VIEW));
	vbView.BufferLocation = m_ptrVertexBuffer->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(VertexForPass) * 4;
	vbView.StrideInBytes = sizeof(VertexForPass);

	// Set topology and view
	ptrCommandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	ptrCommandList->IASetVertexBuffers(0, 1, &vbView);
}
