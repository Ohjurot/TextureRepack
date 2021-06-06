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
	auto t01 = ptrStack->loadFromDisk("./3guys_Box_BaseColor.png");
	auto t02 = ptrStack->loadFromDisk("./3guys_Sphere_BaseColor.png");
	auto t03 = ptrStack->loadFromDisk("./3guys_Torus_BaseColor.png");
	auto tres = ptrStack->createEmpty("./3guys_BaseColor.png", t01);

	// Test loading a modell
	TexRPLib::IGPUGeometryModell* ptrModell = gpuContext->openModell("./3guys.fbx");

	// Create a mask
	TexRPLib::IGPUMask* ptrMask01 = ptrModell->createMask(0, 2048);
	TexRPLib::IGPUMask* ptrMask02 = ptrModell->createMask(1, 2048);
	TexRPLib::IGPUMask* ptrMask03 = ptrModell->createMask(2, 2048);

	// Merge
	UINT input[] = {t01, t02, t03};
	TexRPLib::IGPUMask* masks[] = { ptrMask01, ptrMask02, ptrMask03 };
	gpuContext->mergTextures(masks, ptrStack, tres, 3, input);

	// Download final texture
	ptrStack->safeToDisk(tres);

	// Cleanup
	TexRPDestroy(ptrMask01);
	TexRPDestroy(ptrMask02);
	TexRPDestroy(ptrMask03);
	TexRPDestroy(ptrModell);
	TexRPDestroy(ptrStack);
	TexRPDestroy(gpuContext);
	TexRPDestroy(gpuInterface);

	// Passed OK
	return 0;
}
