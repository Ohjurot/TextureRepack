#include "GPUShader.h"

TexRP::GPUShader::~GPUShader() {
	// Free memory if required
	if (m_ptrShaderBuffer) {
		free(m_ptrShaderBuffer);
	}
}

TexRP::GPUShader::GPUShader(const GPUShader& other) {
	// Check if copy is required
	if (other.m_ptrShaderBuffer) {
		// Copy size and allocate memory
		m_bufferSize = other.m_bufferSize;
		m_ptrShaderBuffer = malloc(m_bufferSize);

		// Copy shader
		if (m_ptrShaderBuffer) {
			memcpy(m_ptrShaderBuffer, other.m_ptrShaderBuffer, m_bufferSize);
		}
	}
}

TexRP::GPUShader::GPUShader(LPCSTR pathToShaderBinary) {	
	// Open shader binary file
	HANDLE hFile = CreateFileA(pathToShaderBinary, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		// Get size of file
		ULARGE_INTEGER fileSize;
		fileSize.LowPart = GetFileSize(hFile, &fileSize.HighPart);
		if (fileSize.QuadPart) {
			// Allocate memory
			m_bufferSize = fileSize.QuadPart;
			m_ptrShaderBuffer = malloc(m_bufferSize);
			if (m_ptrShaderBuffer) {
				// Load file
				BOOL copySuccess = true;
				void* ptrReadBuffer = m_ptrShaderBuffer;
				while (copySuccess && fileSize.QuadPart) {
					// Read file
					const DWORD bytesToRead = fileSize.QuadPart >= DWORD_MAX ? DWORD_MAX : fileSize.LowPart;
					DWORD bytesRead = 0;
					copySuccess = ReadFile(hFile, ptrReadBuffer, bytesToRead, &bytesRead, NULL);

					// Check if copy succeded
					if (copySuccess && bytesToRead == bytesRead) {
						// Move write head
						fileSize.QuadPart -= bytesRead;
						ptrReadBuffer = (void*)((UINT64)ptrReadBuffer + bytesRead);
					}
					else {
						copySuccess = FALSE;
					}
				}

				// Free on failure
				if (!copySuccess) {
					free(m_ptrShaderBuffer);
					m_ptrShaderBuffer = nullptr;
				}
			}
		}
		
		// CLose file 
		CloseHandle(hFile);
	}
}

TexRP::GPUShader& TexRP::GPUShader::operator=(const GPUShader& other) {
	// Check if memory needs to be freed
	if (m_ptrShaderBuffer) {
		free(m_ptrShaderBuffer);
		m_ptrShaderBuffer = nullptr;
	}

	// Copy from other
	m_bufferSize = other.m_bufferSize;

	// Allocate memory if required
	if (other.m_ptrShaderBuffer) {
		m_ptrShaderBuffer = malloc(m_bufferSize);
		if (m_ptrShaderBuffer) {
			// Copy memory
			memcpy(m_ptrShaderBuffer, other.m_ptrShaderBuffer, m_bufferSize);
		}
	}

	// Return self
	return *this;
}

TexRP::GPUShader::operator bool() {
	return m_ptrShaderBuffer != nullptr;
}

const void* TexRP::GPUShader::ptr() {
	return m_ptrShaderBuffer;
}

size_t TexRP::GPUShader::size() {
	// Return size only on valid pointer
	if (m_ptrShaderBuffer) {
		return m_bufferSize;
	}

	// Fallback
	return 0;
}

bool TexRP::GPUShader::isValid() {
	return m_ptrShaderBuffer != nullptr;
}

D3D12_SHADER_BYTECODE TexRP::GPUShader::bytecodeDesc() {
	// Assemble descriptor
	D3D12_SHADER_BYTECODE bc;
	bc.pShaderBytecode = m_ptrShaderBuffer;
	bc.BytecodeLength = m_ptrShaderBuffer ? m_bufferSize : 0;

	// Return bytecode
	return bc;
}
