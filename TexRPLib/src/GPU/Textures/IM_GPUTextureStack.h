#pragma once

#include <TexRPLib/WinInclude.h>
#include <TexRPLib/GPUTextureStack.h>

namespace TexRPLib {
	// Represents and GPU context
	class IM_GPUTextureStack : public IGPUTextureStack{
		public:
			// Destructor
			~IM_GPUTextureStack();

			// Init function
			bool init(IDXGIDevice* ptrDevice);

			// Implement interface 
			

		private:
			// Defines a texture in the stack

			struct STACK_TEXTURE {

			};

			// Heap for textures
			ComPtr<ID3D12Heap> m_textureHeap;

			// TODO: Textures itself
	};
}