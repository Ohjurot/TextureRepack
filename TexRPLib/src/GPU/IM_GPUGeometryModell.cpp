#include "IM_GPUGeometryModell.h"

TexRPLib::IM_GPUGeometryModell::~IM_GPUGeometryModell() {

}

bool TexRPLib::IM_GPUGeometryModell::init(ID3D12Device* ptrDevice, TexRP::GPUCommandList* ptrCommandList, TexRP::GPUCommandQueue* ptrCommandQueue, LPCSTR modellPath, TexRP::Passes::MaskRenderPass* ptrMaskPass) {
	// Store
	m_ptrDevice = ptrDevice;
	m_ptrCommandList = ptrCommandList;
	m_ptrCommandQueue = ptrCommandQueue;
	m_ptrMaskPass = ptrMaskPass;

	// Load modell
	Assimp::Importer importer;
	const aiScene* ptrScene = importer.ReadFile(modellPath, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (!ptrScene) {
		return false;
	}

	// Compute total modell memory requirements
	UINT64 memoryRequiredVertex = 0;
	UINT64 memoryRequiredIndex = 0;
	for (unsigned int i = 0; i < ptrScene->mNumMeshes; i++) {
		// Get pointer to mesh
		const aiMesh* ptrMesh = ptrScene->mMeshes[i];

		// Compute size required for one UV set
		const UINT64 vertexBufferSizePerUV = ptrMesh->mNumVertices * sizeof(float) * 2;

		// Compute number of UV Sets
		UINT8 numberOfUVSets = 0;
		for (unsigned int uv = 0; uv < 8; uv++) {
			if (ptrMesh->mNumUVComponents[uv] == 2) {
				numberOfUVSets++;
			}
		}

		// Compute index buffer size
		for (unsigned int fi = 0; fi < ptrMesh->mNumFaces; fi++) {
			memoryRequiredIndex += ptrMesh->mFaces[fi].mNumIndices * sizeof(unsigned int);
		}

		// Accumlate total memory
		memoryRequiredVertex += numberOfUVSets * vertexBufferSizePerUV;
	}

	// Check memory
	if (!memoryRequiredVertex || !memoryRequiredIndex) {
		return false;
	}

	// Upload Heap properties
	D3D12_HEAP_PROPERTIES propUpload;
	ZeroMemory(&propUpload, sizeof(D3D12_HEAP_PROPERTIES));
	propUpload.Type = D3D12_HEAP_TYPE_UPLOAD;
	propUpload.CreationNodeMask = NULL;
	propUpload.VisibleNodeMask = NULL;

	// Default Heap properties
	D3D12_HEAP_PROPERTIES propDefault;
	ZeroMemory(&propDefault, sizeof(D3D12_HEAP_PROPERTIES));
	propDefault.Type = D3D12_HEAP_TYPE_DEFAULT;
	propDefault.CreationNodeMask = NULL;
	propDefault.VisibleNodeMask = NULL;

	// Describe upload buffer
	D3D12_RESOURCE_DESC uploadBufferDesc;
	ZeroMemory(&uploadBufferDesc, sizeof(D3D12_RESOURCE_DESC));
	uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	uploadBufferDesc.Alignment = 64 * 1024;
	uploadBufferDesc.Width = memoryRequiredVertex + memoryRequiredIndex;
	uploadBufferDesc.Height = 1;
	uploadBufferDesc.DepthOrArraySize = 1;
	uploadBufferDesc.MipLevels = 1;
	uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	uploadBufferDesc.SampleDesc.Count = 1;
	uploadBufferDesc.SampleDesc.Quality = 0;
	uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// Create CPU side d3d buffer
	ComPtr<ID3D12Resource> ptrUploadBuffer;
	if (FAILED(ptrDevice->CreateCommittedResource(&propUpload, D3D12_HEAP_FLAG_NONE, &uploadBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&ptrUploadBuffer)))) {
		return false;
	}

	// Describe vertex buffer
	D3D12_RESOURCE_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D12_RESOURCE_DESC));
	vertexBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexBufferDesc.Alignment = 64 * 1024;
	vertexBufferDesc.Width = memoryRequiredVertex;
	vertexBufferDesc.Height = 1;
	vertexBufferDesc.DepthOrArraySize = 1;
	vertexBufferDesc.MipLevels = 1;
	vertexBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	vertexBufferDesc.SampleDesc.Count = 1;
	vertexBufferDesc.SampleDesc.Quality = 0;
	vertexBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	vertexBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// Create vertex buffer
	if (FAILED(ptrDevice->CreateCommittedResource(&propDefault, D3D12_HEAP_FLAG_NONE, &vertexBufferDesc, D3D12_RESOURCE_STATE_COPY_DEST, NULL, IID_PPV_ARGS(&m_ptrVertexBuffer)))) {
		return false;
	}

	// Describe index buffer
	D3D12_RESOURCE_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D12_RESOURCE_DESC));
	indexBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	indexBufferDesc.Alignment = 64 * 1024;
	indexBufferDesc.Width = memoryRequiredIndex;
	indexBufferDesc.Height = 1;
	indexBufferDesc.DepthOrArraySize = 1;
	indexBufferDesc.MipLevels = 1;
	indexBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	indexBufferDesc.SampleDesc.Count = 1;
	indexBufferDesc.SampleDesc.Quality = 0;
	indexBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	indexBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// Create vertex buffer
	if (FAILED(ptrDevice->CreateCommittedResource(&propDefault, D3D12_HEAP_FLAG_NONE, &indexBufferDesc, D3D12_RESOURCE_STATE_COPY_DEST, NULL, IID_PPV_ARGS(&m_ptrIndexBuffer)))) {
		return false;
	}

	// Mapping buffer to application
	BYTE* ptrUploadBufferData;
	if (FAILED(ptrUploadBuffer->Map(NULL, NULL, (void**)&ptrUploadBufferData))) {
		return false;
	}

	// Fill buffer
	UINT64 vertexOffset = 0;
	UINT64 indexOffset = 0;
	for (unsigned int sm = 0; sm < ptrScene->mNumMeshes && m_meshCount < __TEXRPLIB_IM_GEOMODELL_MAXMESHCOUNT; sm++) {
		const aiMesh* ptrMesh = ptrScene->mMeshes[sm];
		
		// Mesh template
		MeshInfo meshTemplate;
		ZeroMemory(&meshTemplate, sizeof(MeshInfo));

		// Input indicies information
		strcpy_s<256>(meshTemplate.name, ptrMesh->mName.C_Str());
		meshTemplate.indexOffset = indexOffset;
		meshTemplate.indexCount = 0;

		// Assemble indicies
		for (unsigned int fi = 0; fi < ptrMesh->mNumFaces; fi++) {
			// Copy and increment
			const UINT64 copySize = sizeof(unsigned int) * ptrMesh->mFaces[fi].mNumIndices;
			memcpy(&ptrUploadBufferData[memoryRequiredVertex + indexOffset], ptrMesh->mFaces[fi].mIndices, copySize);
			meshTemplate.indexCount += ptrMesh->mFaces[fi].mNumIndices;
			indexOffset += copySize;
		}

		// For each uv set
		UINT uvSetIndex = 0;
		for (unsigned int uv = 0; uv < 8; uv++) {
			if (ptrMesh->mNumUVComponents[uv] == 2) {
				// Allocate meta info slot (From template)
				MeshInfo* ptrMeshInfo = &m_meshInfos[m_meshCount++];
				memcpy(ptrMeshInfo, &meshTemplate, sizeof(MeshInfo));

				// Fill mesh info
				ptrMeshInfo->uvSet = uvSetIndex++;
				ptrMeshInfo->vertexOffset = vertexOffset;
				ptrMeshInfo->vertexCount = ptrMesh->mNumVertices;

				// Get pointer to associated uv data
				float* ptrUvSetData = (float*)&ptrUploadBufferData[vertexOffset];
				vertexOffset += sizeof(float) * 2 * ptrMeshInfo->vertexCount;

				// Copy verticies (UV)
				for (unsigned int i = 0; i < ptrMeshInfo->vertexCount; i++) {
					ptrUvSetData[0] = ptrMesh->mTextureCoords[uv][i].x;
					ptrUvSetData[1] = ptrMesh->mTextureCoords[uv][i].y;
					ptrUvSetData = &ptrUvSetData[2];
				}
			}
		}
	}

	// Unmap
	ptrUploadBuffer->Unmap(NULL, NULL);

	// Copy to GPU
	ptrCommandList->ptr()->CopyBufferRegion(m_ptrVertexBuffer.Get(), 0, ptrUploadBuffer.Get(), 0, memoryRequiredVertex);
	ptrCommandList->ptr()->CopyBufferRegion(m_ptrIndexBuffer.Get(), 0, ptrUploadBuffer.Get(), memoryRequiredVertex, memoryRequiredIndex);

	// Resource barrier vertex buffer
	D3D12_RESOURCE_BARRIER bufferBarrieres[2];
	ZeroMemory(&bufferBarrieres[0], sizeof(D3D12_RESOURCE_BARRIER));
	bufferBarrieres[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	bufferBarrieres[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	bufferBarrieres[0].Transition.pResource = m_ptrVertexBuffer.Get();
	bufferBarrieres[0].Transition.Subresource = NULL;
	bufferBarrieres[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	bufferBarrieres[0].Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;

	// Resource barrier vertex buffer
	ZeroMemory(&bufferBarrieres[1], sizeof(D3D12_RESOURCE_BARRIER));
	bufferBarrieres[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	bufferBarrieres[1].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	bufferBarrieres[1].Transition.pResource = m_ptrIndexBuffer.Get();
	bufferBarrieres[1].Transition.Subresource = NULL;
	bufferBarrieres[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	bufferBarrieres[1].Transition.StateAfter = D3D12_RESOURCE_STATE_INDEX_BUFFER;
	
	// Queue barrieres
	ptrCommandList->ptr()->ResourceBarrier(2, bufferBarrieres);

	// Executeque
	ID3D12CommandList* executionArray[] = { *ptrCommandList };
	ptrCommandList->close();
	ptrCommandQueue->dispatchSync(executionArray, 1);
	ptrCommandList->reset();

	// Pass
	return true;
}

bool TexRPLib::IM_GPUGeometryModell::createViews(UINT index, D3D12_VERTEX_BUFFER_VIEW* ptrVertexBufferView, D3D12_INDEX_BUFFER_VIEW* ptrIndexBufferView, UINT64* ptrIndexCount) {
	// Bound check
	if (index >= m_meshCount) {
		return false;
	}

	// Create VB View
	ptrVertexBufferView->BufferLocation = m_ptrVertexBuffer->GetGPUVirtualAddress() + m_meshInfos[index].vertexOffset;
	ptrVertexBufferView->SizeInBytes = m_meshInfos[index].vertexCount * sizeof(float) * 2;
	ptrVertexBufferView->StrideInBytes = sizeof(float) * 2;

	// Create IB View
	ptrIndexBufferView->BufferLocation = m_ptrIndexBuffer->GetGPUVirtualAddress() + m_meshInfos[index].indexOffset;
	ptrIndexBufferView->Format = DXGI_FORMAT_R32_UINT;
	ptrIndexBufferView->SizeInBytes = sizeof(unsigned int) * m_meshInfos[index].indexCount;

	// Set index count
	*ptrIndexCount = m_meshInfos[index].indexCount;

	// Pass
	return true;
}

UINT TexRPLib::IM_GPUGeometryModell::getSubmodellCount() {
	return m_meshCount;
}

LPCSTR TexRPLib::IM_GPUGeometryModell::getSubmodellName(UINT index) {
	// Return name if in range
	if (index < m_meshCount) {
		return m_meshInfos[index].name;
	}
	
	// Fallback
	return nullptr;
}

UINT TexRPLib::IM_GPUGeometryModell::getUVSetIndex(UINT index) {
	// Return name if in range
	if (index < m_meshCount) {
		return m_meshInfos[index].uvSet;
	}

	// Fallback
	return UINT_MAX;
}

UINT TexRPLib::IM_GPUGeometryModell::getVerticesCount(UINT index) {
	// Return name if in range
	if (index < m_meshCount) {
		return m_meshInfos[index].vertexCount;
	}

	// Fallback
	return 0;
}

TexRPLib::IGPUMask* TexRPLib::IM_GPUGeometryModell::createMask(UINT index, UINT sideLength) {
	// Bound check
	if (index < m_meshCount) {
		// Create mask object
		IM_GPUMask* ptrMask = TexRPAllocate(IM_GPUMask, IM_GPUMask);
		if (ptrMask->init(m_ptrDevice.Get(), sideLength)) {
			// Get geometry data
			D3D12_VERTEX_BUFFER_VIEW vbView;
			D3D12_INDEX_BUFFER_VIEW ibView;
			UINT64 indexCount;
			createViews(index, &vbView, &ibView, &indexCount);
			
			// Bind render pass
			m_ptrMaskPass->bind(*m_ptrCommandList);

			// Bind mask
			ptrMask->bindAsRTV(*m_ptrCommandList);

			// Prepare mask render
			m_ptrCommandList->ptr()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_ptrCommandList->ptr()->IASetVertexBuffers(0, 1, &vbView);
			m_ptrCommandList->ptr()->IASetIndexBuffer(&ibView);

			// Draw
			m_ptrCommandList->ptr()->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);

			// Queue execute sync
			m_ptrCommandList->close();
			ID3D12CommandList* execArray[] = { *m_ptrCommandList };
			m_ptrCommandQueue->dispatchSync(execArray, 1);
			m_ptrCommandList->reset();

			// Return list
			return ptrMask;
		}
		
		// Destroy mask (if not returned)
		TexRPDestroy(ptrMask);
	}
	
	// Fallback
	return nullptr;
}
