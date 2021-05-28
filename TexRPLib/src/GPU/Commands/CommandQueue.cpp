#include "CommandQueue.h"

bool TexRP::GPUCommandQueue::init(ID3D12Device* ptrDevice, D3D12_COMMAND_LIST_TYPE type){
	// Create queue decriptor
	D3D12_COMMAND_QUEUE_DESC qd;
	ZeroMemory(&qd, sizeof(D3D12_COMMAND_QUEUE_DESC));
	
	// Describe queue
	qd.Type = type;
	qd.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	qd.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	qd.NodeMask = NULL;

	// Create command queue
	if (FAILED(ptrDevice->CreateCommandQueue(&qd, IID_PPV_ARGS(&m_ptrCommandQueue)))) {
		return false;
	}

	// Create fence for queue
	if (FAILED(ptrDevice->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_ptrFence)))) {
		return false;
	}

	// Passed
	return true;
}

void TexRP::GPUCommandQueue::dispatchSync(ID3D12CommandList** arrCmdLists, UINT numLists) {
	// Disptach async
	UINT64 flag = dispatchAsync(arrCmdLists, numLists);
	// Wait sync
	waitSync(flag);
}

UINT64 TexRP::GPUCommandQueue::dispatchAsync(ID3D12CommandList** arrCmdLists, UINT numLists) {
	// Dispatch lists
	m_ptrCommandQueue->ExecuteCommandLists(numLists, (ID3D12CommandList* const*)arrCmdLists);
	
	// Signal for new fence value and return it
	m_ptrCommandQueue->Signal(m_ptrFence.Get(), ++m_fenceValue);
	return m_fenceValue;
}

void TexRP::GPUCommandQueue::waitSync(UINT64 waitFlag) {
	// Only enter the waiting madness in case we need to wait
	if (!waitPeek(waitFlag)) {
		// Create event for waiting
		HANDLE eventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (eventHandle) {
			// Subscribe to fence
			if (SUCCEEDED(m_ptrFence->SetEventOnCompletion(waitFlag, eventHandle))) {
				// Wait for fence
				if (WaitForSingleObject(eventHandle, INFINITE) == WAIT_OBJECT_0) {
					// Finished waiting
					return;
				}
			}
		}

		// Fallback c++ wait loop
		while (!waitPeek(waitFlag)) {
			SwitchToThread();
		}
	}
}

bool TexRP::GPUCommandQueue::waitPeek(UINT64 waitFlag) {
	return m_ptrFence->GetCompletedValue() >= waitFlag;
}

void TexRP::GPUCommandQueue::release() {
	m_ptrFence.ReleaseAndGetAddressOf();
	m_ptrCommandQueue.ReleaseAndGetAddressOf();
}

ID3D12CommandQueue* TexRP::GPUCommandQueue::operator->() {
	return m_ptrCommandQueue.Get();
}

TexRP::GPUCommandQueue::operator ID3D12CommandQueue* () {
	return m_ptrCommandQueue.Get();
}
