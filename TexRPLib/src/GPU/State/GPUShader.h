#pragma once

#include <TexRPLib/TEXRP.h>
#include <TexRPLib/WinInclude.h>

namespace TexRP {
	// Represents a shader on the GPU
	class GPUShader {
		public:
			// Default constructor and custom destructor
			GPUShader() = default;
			~GPUShader();

			// Special constructors
			GPUShader(const GPUShader& other);
			GPUShader(LPCSTR pathToShaderBinary);

			// Copy operator(s)
			GPUShader& operator=(const GPUShader& other);

			// Conversion operators
			operator bool();

			// Get buffer pointer
			const void* ptr();
			// Get buffer size
			size_t size();
			// Checks if object is valid
			bool isValid();
			// Creates a bytecode descritor for this shader
			D3D12_SHADER_BYTECODE bytecodeDesc();

		private:
			// Represents the raw shader memory
			void* m_ptrShaderBuffer = nullptr;

			// Represents the size of the shader buffer
			size_t m_bufferSize = 0;
	};
}