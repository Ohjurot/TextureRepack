#pragma once

#include <TexRPLib/TEXRP.h>
#include <TexRPLib/WinInclude.h>
#include <TexRPLib/GPUGeometryModell.h>

#include <GPU/Commands/CommandList.h>
#include <GPU/Commands/CommandQueue.h>
#include <GPU/IM_GPUMask.h>

#include <GPU/Passes/MaskRenderPass.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#define __TEXRPLIB_IM_GEOMODELL_MAXMESHCOUNT 128

namespace TexRPLib {
	// Implementation of a geometry modell
	class IM_GPUGeometryModell : public IGPUGeometryModell {
		public:
			// Destructor
			~IM_GPUGeometryModell() override;

			// Init function
			bool init(ID3D12Device* ptrDevice, TexRP::GPUCommandList* ptrCommandList, TexRP::GPUCommandQueue* ptrCommandQueue, LPCSTR modellPath, TexRP::Passes::MaskRenderPass* ptrMaskPass);
			
			// Function to create view to geometry (at index)
			bool createViews(UINT index, D3D12_VERTEX_BUFFER_VIEW* ptrVertexBufferView, D3D12_INDEX_BUFFER_VIEW* ptrIndexBufferView, UINT64* ptrIndexCount);

			// Implement interface
			UINT getSubmodellCount() override;
			LPCSTR getSubmodellName(UINT index) override;
			UINT getUVSetIndex(UINT index) override;
			UINT getVerticesCount(UINT index) override;
			TexRPLib::IGPUMask* createMask(UINT index, UINT sideLength) override;

		private:
			// Basic mesh info
			struct MeshInfo {
				// Name of mesh
				CHAR name[256];

				// UV Set representd by this struct
				UINT uvSet;

				// Vertex meta data
				UINT64 vertexOffset, vertexCount;

				// Index meta data
				UINT64 indexOffset, indexCount;
			};

		private:
			// Device pointer
			ComPtr<ID3D12Device> m_ptrDevice;

			// Buffer storing all geometry
			ComPtr<ID3D12Resource> m_ptrVertexBuffer, m_ptrIndexBuffer;

			// Command list and queue
			TexRP::GPUCommandList* m_ptrCommandList = nullptr;
			TexRP::GPUCommandQueue* m_ptrCommandQueue = nullptr;

			// Pass
			TexRP::Passes::MaskRenderPass* m_ptrMaskPass;

			// Mesh information
			MeshInfo m_meshInfos[__TEXRPLIB_IM_GEOMODELL_MAXMESHCOUNT];

			// Submeshes occupied
			UINT16 m_meshCount = 0;
	};
}