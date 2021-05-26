#include <cstdint>
#include <iostream>

#include <TexRPLib/GPUInterface.h>

// Application entry point
int main(int argc, char** argv) {
	// Get and echo interface
	TexRPLib::IGPUInterface* gpuInterface = TexRPLib::enumGpuInterfaces(0);
	std::wcout << L"GPU Name: " << gpuInterface->getGPUName() << std::endl << L"Current OS Memory Budge: " << gpuInterface->getGPUFreeVRAM() / 1024 / 1024 << L" MB" << std::endl;
	
	// Create gpu context
	TexRPLib::IGPUContext* gpuContext = gpuInterface->createContex();
	
	
	// Cleanup
	TexRPDestroy(gpuContext);
	TexRPDestroy(gpuInterface);

	// Passed OK
	return 0;
}
