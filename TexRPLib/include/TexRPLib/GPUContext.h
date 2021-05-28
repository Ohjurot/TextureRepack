#pragma once

#include <TexRPLib/WinInclude.h>

namespace TexRPLib {
	class IGPUContext {
		public:
			// Virtual destructor
			virtual ~IGPUContext() {};

			// Check if the format is supported as input format (shader resource view)
			virtual bool checkInputSupport(DXGI_FORMAT format) = 0;

			// Check if the format is supported as output format (render target)
			virtual bool checkOutputSupport(DXGI_FORMAT format) = 0;
	};
}
