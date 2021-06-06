#pragma once

#include <TexRPLib/TEXRP.h>
#include <TexRPLib/WinInclude.h>

#include <TexRPLib/GPUMask.h>

namespace TexRPLib {
	// Represents a geometric modell
	class IGPUGeometryModell {
		public:
			// Virtual destructor
			virtual ~IGPUGeometryModell() {};

			// Number of submodells
			virtual UINT getSubmodellCount() = 0;

			// Get name of submodell
			virtual LPCSTR getSubmodellName(UINT index) = 0;

			// Get the uv set indesx assoicated with that geometry slot
			virtual UINT getUVSetIndex(UINT index) = 0;

			// Get vertices count
			virtual UINT getVerticesCount(UINT index) = 0;

			// Create mask from geometry
			virtual TexRPLib::IGPUMask* createMask(UINT index, UINT sideLength) = 0;
	};
}
