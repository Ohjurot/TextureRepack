#pragma once

#include <TexRPLib/TEXRP.h>
#include <TexRPLib/WinInclude.h>

#include <GPU/Passes/IPass.h>
#include <GPU/State/GPUShader.h>

#include <GPU/Commands/CommandQueue.h>
#include <GPU/Commands/CommandList.h>

namespace TexRP {
	namespace Passes {
		// Object for a mask render pass
		class MergeRenderPass : public IPass{
			public:
				// Destruct
				~MergeRenderPass();

				// Init the mask render pass
				bool init(ID3D12Device* ptrDevice, TexRP::GPUCommandQueue* ptrCommandQueue, TexRP::GPUCommandList* ptrCommandList);
				void release();

				// Interface
				void bind(ID3D12GraphicsCommandList* ptrCommandList) override;

			private:
				// Struct for vertex data
				struct VertexForPass {
					float x, y;
				};

				// Actual vertex data
				const VertexForPass c_verticies[4] = {
					{-1.0f,	-1.0f},
					{-1.0f,	 1.0f},
					{ 1.0f, -1.0f},
					{ 1.0f,  1.0f}
				};

			private:
				// Shaders
				GPUShader m_rootSignature;
				GPUShader m_vertexShader;
				GPUShader m_pixelShader;

				// Vertex buffer
				ComPtr<ID3D12Resource> m_ptrVertexBuffer;

				// Root Signature
				ComPtr<ID3D12RootSignature> m_ptrRootSignature;

				// PSO
				ComPtr<ID3D12PipelineState> m_ptrPSO;
		};
	}
}