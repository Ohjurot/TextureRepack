#pragma once

#include <TexRPLib/WinInclude.h>
#include <TexRPLib/GPUTextureStack.h>

#include <GPU/Commands/CommandQueue.h>
#include <GPU/Commands/CommandList.h>
#include <CPU/Textures/TextureMeta.h>
#include <CPU/Textures/TextureIO.h>
#include <CPU/Textures/TextureTools.h>

#include <Util/MallocdMemory.h>

namespace TexRPLib {
	// Represents and GPU context
	class IM_GPUTextureStack : public IGPUTextureStack{
		public:
			// Destructor
			~IM_GPUTextureStack();

			// Init function
			bool init(ID3D12Device* ptrDevice, TexRP::GPUCommandQueue* ptrCmdQueue, TexRP::GPUCommandList* ptrCmdList);
			void release();

			// Implement interface 
			bool reset(UINT width, UINT height, UINT bpp, UINT count) override;
			UINT loadFromDisk(LPCSTR path) override;
			UINT createEmpty(LPCSTR dummyPath, UINT reference) override;
			bool safeToDisk(UINT index) override;
			bool rename(UINT index, LPCSTR newName) override;

		private:
			// Defines a texture in the stack
			struct STACK_TEXTURE {
				// Final texture on the gpu
				ComPtr<ID3D12Resource> gpuResource;

				// All meta data for this texture
				TexRP::TEXTURE_INFO info;
			};

			// Copy of device pointer
			ComPtr<ID3D12Device> m_device;

			// Executing command queue
			TexRP::GPUCommandQueue* m_ptrCmdQueue;
			
			// Executing command list
			TexRP::GPUCommandList* m_ptrCmdList;

			// Heap for textures
			ComPtr<ID3D12Heap> m_textureHeap;

			// Textures for Up/Down loading texture
			ComPtr<ID3D12Resource> m_uploadTexture, m_downloadTexture;

			// Total size of heap
			UINT64 m_heapSize = 0;

			// Next free byte of heap
			UINT64 m_headHead = 0;

			// Number of texture used
			UINT m_textureUsed = 0;

			// Array of texture
			STACK_TEXTURE m_arrTexture[256];
	};
}