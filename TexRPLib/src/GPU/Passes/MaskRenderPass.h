#pragma once

#include <TexRPLib/TEXRP.h>
#include <TexRPLib/WinInclude.h>

#include <GPU/Passes/IPass.h>
#include <GPU/State/GPUShader.h>

namespace TexRP {
	namespace Passes {
		// Object for a mask render pass
		class MaskRenderPass : public IPass{
			public:
				// Destruct
				~MaskRenderPass();

				// Init the mask render pass
				bool init(ID3D12Device* ptrDevice);
				void release();

				// Interface
				void bind(ID3D12GraphicsCommandList* ptrCommandList) override;

			private:
				// Shaders
				GPUShader m_rootSignature;
				GPUShader m_vertexShader;
				GPUShader m_pixelShader;

				// Root Signature
				ComPtr<ID3D12RootSignature> m_ptrRootSignature;

				// PSO
				ComPtr<ID3D12PipelineState> m_ptrPSO;
		};
	}
}