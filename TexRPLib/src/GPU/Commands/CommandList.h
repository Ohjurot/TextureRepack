#pragma once

#include <TexRPLib/WinInclude.h>

namespace TexRP {
	// Represents a command list on the gpu
	class GPUCommandList {
		public:
			// Init the class
			bool init(ID3D12Device* ptrDevice, ID3D12CommandQueue* ptrCommandQueue, D3D12_COMMAND_LIST_TYPE type);

			// Pre destructor release
			void release();

			// Close command list
			void close();

			// Reset command list
			void reset();

			// Getters
			ID3D12GraphicsCommandList* ptr();

			// Operator overloading
			ID3D12GraphicsCommandList* operator->();
			operator ID3D12GraphicsCommandList* ();
		private:
			// Command allocator
			ComPtr<ID3D12CommandAllocator> m_ptrCommandAllocator;

			// Command list
			ComPtr<ID3D12GraphicsCommandList> m_ptrCommandList;
	};
}
