#pragma once

#include <TexRPLib/WinInclude.h>
#include <TexRPLib/GPUContext.h>

#include <GPU/Commands/CommandQueue.h>
#include <GPU/Commands/CommandList.h>

#include <GPU/Textures/IM_GPUTextureStack.h>
#include <GPU/IM_GPUGeometryModell.h>

#include <DXProgrammableCapture.h>

#include <GPU/Passes/MaskRenderPass.h>

// DEBUG INCLUDES

// END DEBUG

namespace TexRPLib {
	// Represents and GPU context
	class IM_GPUContext : public IGPUContext{
		public:
			// Destructor
			~IM_GPUContext();

			// Init function
			bool init(IDXGIAdapter* ptrAdpter);

			// Implement interface 
			bool checkInputSupport(DXGI_FORMAT format) override;
			bool checkOutputSupport(DXGI_FORMAT format) override;
			TexRPLib::IGPUTextureStack* createTextureStack() override;
			TexRPLib::IGPUGeometryModell* openModell(LPCSTR modellPath) override;

		private:
			// Main DirectX Device
			ComPtr<ID3D12Device> m_ptrDevice;
			#ifdef _DEBUG
			ComPtr<ID3D12DebugDevice> m_ptrDebugDevice;
			ComPtr<IDXGraphicsAnalysis> m_pixCaptureControle;
			#endif			

			// Command queue for command staging
			TexRP::GPUCommandQueue m_directCommandQueue;

			// Command list for recording commands on the gpu
			TexRP::GPUCommandList m_directCommandList;

			// Render passes
			TexRP::Passes::MaskRenderPass m_maskPass;
	};
}