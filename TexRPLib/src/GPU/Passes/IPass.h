#pragma once

#include <TexRPLib/TEXRP.h>
#include <TexRPLib/WinInclude.h>

namespace TexRP {
	namespace Passes {
		class IPass {
			public:
				// Bind the pass to a command list
				virtual void bind(ID3D12GraphicsCommandList* ptrCommandList) = 0;
		};
	}
}