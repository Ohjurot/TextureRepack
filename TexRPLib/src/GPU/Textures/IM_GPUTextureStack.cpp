#include "IM_GPUTextureStack.h"

TexRPLib::IM_GPUTextureStack::~IM_GPUTextureStack() {
	release();
}

bool TexRPLib::IM_GPUTextureStack::init(ID3D12Device* ptrDevice, TexRP::GPUCommandQueue* ptrCmdQueue, TexRP::GPUCommandList* ptrCmdList) {
	// Set and done
	m_device = ptrDevice;
	m_ptrCmdQueue = ptrCmdQueue;
	m_ptrCmdList = ptrCmdList;
	return true;
}

void TexRPLib::IM_GPUTextureStack::release() {
	// Release textures
	for (unsigned int i = 0; i < m_textureUsed; i++) {
		m_arrTexture[i].gpuResource.ReleaseAndGetAddressOf();
	}
	m_textureUsed = 0;

	// Release heap
	m_textureHeap.ReleaseAndGetAddressOf();
	m_heapSize = 0;
	m_headHead = 0;

	// Release up/down texture
	m_downloadTexture.ReleaseAndGetAddressOf();
	m_uploadTexture.ReleaseAndGetAddressOf();
}

bool TexRPLib::IM_GPUTextureStack::reset(UINT width, UINT height, UINT bpp, UINT count) {
	// Release old members
	release();
	
	// Calculating heap sizes
	const UINT bytesPerPixel = (bpp + 7) / 8;
	const UINT64 heapSize = width * height * bytesPerPixel * count;

	// TODO: Ask if memory is availible

	// Describe heap
	D3D12_HEAP_DESC hd;
	ZeroMemory(&hd, sizeof(D3D12_HEAP_DESC));
	hd.SizeInBytes = heapSize;
	hd.Alignment = 64 * 1024;
	hd.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
	hd.Properties.VisibleNodeMask = NULL;
	hd.Properties.CreationNodeMask = NULL;
	hd.Flags = D3D12_HEAP_FLAG_NONE;

	// Create heap for texture
	if (FAILED(m_device->CreateHeap(&hd, IID_PPV_ARGS(&m_textureHeap)))) {
		return false;
	}

	// Describe resource
	D3D12_RESOURCE_DESC rd;
	rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	rd.Alignment = 64 * 1024;
	rd.Width = width * height * bytesPerPixel;
	rd.Height = 1;
	rd.DepthOrArraySize = 1;
	rd.MipLevels = 1;
	rd.Format = DXGI_FORMAT_UNKNOWN;
	rd.SampleDesc.Count = 1;
	rd.SampleDesc.Quality = 0;
	rd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	rd.Flags = D3D12_RESOURCE_FLAG_NONE;

	// Upload heap propertie
	D3D12_HEAP_PROPERTIES propUpload;
	ZeroMemory(&propUpload, sizeof(D3D12_HEAP_PROPERTIES));
	propUpload.Type = D3D12_HEAP_TYPE_UPLOAD;
	propUpload.CreationNodeMask = NULL;
	propUpload.VisibleNodeMask = NULL;

	// Download heap propertie
	D3D12_HEAP_PROPERTIES propDownload;
	ZeroMemory(&propDownload, sizeof(D3D12_HEAP_PROPERTIES));
	propDownload.Type = D3D12_HEAP_TYPE_READBACK;
	propDownload.CreationNodeMask = NULL;
	propDownload.VisibleNodeMask = NULL;

	// Create Up/Down textures
	if (
		FAILED(m_device->CreateCommittedResource(&propUpload, D3D12_HEAP_FLAG_NONE, &rd, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&m_uploadTexture))) ||
		FAILED(m_device->CreateCommittedResource(&propDownload, D3D12_HEAP_FLAG_NONE, &rd, D3D12_RESOURCE_STATE_COPY_DEST, NULL, IID_PPV_ARGS(&m_downloadTexture)))
	) {
		return false;
	}

	// Store creation results
	m_heapSize = heapSize;

	// Pass
	return true;
}

UINT TexRPLib::IM_GPUTextureStack::loadFromDisk(LPCSTR path) {
	// Check bounds
	if (m_textureUsed >= 256 || !m_heapSize) {
		return UINT_MAX;
	}
	
	// Get texture meta
	TexRP::TEXTURE_INFO info;
	if (!TexRP::TextureIO::readTextureInformationFromFile(path, &info.metaInfo, &info.sourceFormat)) {
		return UINT_MAX;
	}

	// Translate into directx format
	if (!TexRP::TextureTools::matchFormat(info.sourceFormat, &info.targetFormat)) {
		return UINT_MAX;
	}

	// Creating resource description for that
	D3D12_RESOURCE_DESC rd;
	ZeroMemory(&rd, sizeof(D3D12_RESOURCE_DESC));
	
	// Describe texture
	rd.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	rd.Alignment = 64 * 1024;
	rd.Width = info.metaInfo.width;
	rd.Height = info.metaInfo.height;
	rd.DepthOrArraySize = 1;
	rd.MipLevels = 1;
	rd.Format = info.targetFormat.d3dFormat;
	rd.SampleDesc.Count = 1;
	rd.SampleDesc.Quality = 0;
	rd.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	rd.Flags = D3D12_RESOURCE_FLAG_NONE;

	// Get size of texure
	const UINT64 textureSize = m_device->GetResourceAllocationInfo(NULL, 1, &rd).SizeInBytes;
	const UINT bytesPerPixel = (info.targetFormat.bpp + 7) / 8;

	// Chck if heap can hold texture
	if (textureSize > (m_heapSize - m_headHead)) {
		return UINT_MAX;
	}

	// Allocate index
	const UINT resourceIndex = m_textureUsed++;

	// Create actual texure
	if (FAILED(m_device->CreatePlacedResource(
		m_textureHeap.Get(), m_headHead,
		&rd, D3D12_RESOURCE_STATE_COPY_DEST, NULL,
		IID_PPV_ARGS(&m_arrTexture[resourceIndex].gpuResource)
	))) {
		return UINT_MAX;
	}

	// Increment heap head
	m_headHead += textureSize;

	// Map texture
	void* cpuTexture = nullptr;
	if (FAILED(m_uploadTexture->Map(NULL, NULL, &cpuTexture))) {
		return UINT_MAX;
	}

	// Loading the texture
	if (!TexRP::TextureIO::loadTexture(info.metaInfo, info.sourceFormat, info.targetFormat, cpuTexture, info.metaInfo.width * info.metaInfo.height * bytesPerPixel)) {
		return UINT_MAX;
	}

	// Unmap texture
	m_uploadTexture->Unmap(NULL, NULL);

	// Copy members
	D3D12_TEXTURE_COPY_LOCATION copySrc, copyDest;

	// Src
	copySrc.pResource = m_uploadTexture.Get();
	copySrc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	copySrc.PlacedFootprint.Offset = 0;
	copySrc.PlacedFootprint.Footprint.Width = info.metaInfo.width;
	copySrc.PlacedFootprint.Footprint.Height = info.metaInfo.height;
	copySrc.PlacedFootprint.Footprint.Depth = 1;
	copySrc.PlacedFootprint.Footprint.RowPitch = info.metaInfo.width * bytesPerPixel;
	copySrc.PlacedFootprint.Footprint.Format = info.targetFormat.d3dFormat;

	// Dest
	copyDest.pResource = m_arrTexture[resourceIndex].gpuResource.Get();
	copyDest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	copyDest.SubresourceIndex = NULL;

	D3D12_BOX srcBox;
	srcBox.left = 0;
	srcBox.top = 0;
	srcBox.front = 0;
	srcBox.right = info.metaInfo.width;
	srcBox.bottom = info.metaInfo.height;
	srcBox.back = 1;

	// Copy
	m_ptrCmdList->ptr()->CopyTextureRegion(&copyDest, 0, 0, 0, &copySrc, &srcBox);

	// Make texture usable
	D3D12_RESOURCE_BARRIER barr;
	ZeroMemory(&barr, sizeof(D3D12_RESOURCE_BARRIER));
	barr.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barr.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barr.Transition.pResource = m_arrTexture[resourceIndex].gpuResource.Get();
	barr.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barr.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barr.Transition.Subresource = NULL;
	m_ptrCmdList->ptr()->ResourceBarrier(1, &barr);

	// Execute
	ID3D12CommandList* arrX[] = { m_ptrCmdList->ptr() };
	m_ptrCmdList->close();
	m_ptrCmdQueue->dispatchSync(arrX, 1);
	m_ptrCmdList->reset();

	// Return final index
	return resourceIndex;
}

UINT TexRPLib::IM_GPUTextureStack::createEmpty(LPCSTR dummyPath, UINT reference)
{
	return 0;
}

UINT TexRPLib::IM_GPUTextureStack::safeToDisk(UINT index)
{
	return 0;
}
 