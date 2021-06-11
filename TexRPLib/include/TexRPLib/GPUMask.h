#pragma once

#include <TexRPLib/TEXRP.h>
#include <TexRPLib/WinInclude.h>

namespace TexRPLib {
	class IGPUMask : public ITexRPObject {
		public:
			// Virtual destructor
			virtual ~IGPUMask() {}

			// Clear the mask
			virtual void clear() = 0;
	};
}
