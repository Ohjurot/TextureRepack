#pragma once

#include <TexRPLib/TEXRP.h>
#include <TexRPLib/WinInclude.h>

#include <TexRPLib/GPUMask.h>

#include <GPU/Commands/CommandQueue.h>
#include <GPU/Commands/CommandList.h>

namespace TexRPLib {
	class IM_GPUMask : public IGPUMask {
		public:
			// Destructor
			~IM_GPUMask() override;

			// Init function
			bool init(ID3D12Device* ptrDevice, UINT sideLength);

			// Functions
			// Bind the mask as a srv resource
			void bindAsRTV(ID3D12GraphicsCommandList* ptrCommandList);
			// TODO: Retriver SRV handle


			// Implement interface
			void clear();

		private:
			// The mask resorurce
			ComPtr<ID3D12Resource> m_ptrMaskResource;

			// Descritor heaps
			ComPtr<ID3D12DescriptorHeap> m_ptrRTVHeap;

			// Side length
			UINT m_sideLength;

			// Clear flag
			bool m_shouldClear = true;

			// Current resource state
			D3D12_RESOURCE_STATES m_currentState;
	};
}