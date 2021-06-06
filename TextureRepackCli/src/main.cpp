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

	// Reserve memory
	auto reserveOk = gpuInterface->setVRAMReservation(1024ULL * 1024ULL * 1024ULL * 4ULL);
	auto reserverFeedback = gpuInterface->getVRAMReservation();

	// Check support for common formats
	DXGI_FORMAT fmts[] = {
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R16G16B16A16_UNORM,
		DXGI_FORMAT_R8_UNORM,
	};
	for (unsigned int i = 0; i < _countof(fmts); i++) {
		std::wcout << L"[" << i + 1 << L"] Input: " << gpuContext->checkInputSupport(fmts[i]) << L" Output: " << gpuContext->checkOutputSupport(fmts[i]) << std::endl;
	}

	// Allocate texture stack
	TexRPLib::IGPUTextureStack* ptrStack = gpuContext->createTextureStack();
	ptrStack->reset(2048, 2048, 32, 32);

	// Test loading and saving a texture
	auto tid = ptrStack->loadFromDisk("./3guys_Box_BaseColor.png");
	ptrStack->rename(tid, "./color_copy.png");
	ptrStack->safeToDisk(tid);

	// Test loading a modell
	TexRPLib::IGPUGeometryModell* ptrModell = gpuContext->openModell("./3guys.fbx");

	// Create a mask
	TexRPLib::IGPUMask* ptrMask = ptrModell->createMask(2, 2048);


	// Cleanup
	TexRPDestroy(ptrMask);
	TexRPDestroy(ptrModell);
	TexRPDestroy(ptrStack);
	TexRPDestroy(gpuContext);
	TexRPDestroy(gpuInterface);

	// Passed OK
	return 0;
}
