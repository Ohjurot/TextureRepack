#pragma once

#include <TexRPLib/WinInclude.h>

namespace TexRP{
	// Represents a command queue on the gpu
	class GPUCommandQueue {
		public:
			// Init the class
			bool init(ID3D12Device* ptrDevice, D3D12_COMMAND_LIST_TYPE type);

			// Dispatch sync (blocking)
			void dispatchSync(ID3D12CommandList** arrCmdLists, UINT numLists);

			// Dispatches async (non blocking)
			UINT64 dispatchAsync(ID3D12CommandList** arrCmdLists, UINT numLists);

			// Wait sync until work from async is done
			void waitSync(UINT64 waitFlag);

			// Peek if an wait operation is currently done
			bool waitPeek(UINT64 waitFlag);

			// Pre destructor release
			void release();

			// Operator overloading
			ID3D12CommandQueue* operator->();
			operator ID3D12CommandQueue* ();
		private:
			// Queu to push commands on
			ComPtr<ID3D12CommandQueue> m_ptrCommandQueue;
			
			// Fence to wait for command execution
			ComPtr<ID3D12Fence> m_ptrFence;

			// Fence value
			UINT64 m_fenceValue = 0;
	};
}
