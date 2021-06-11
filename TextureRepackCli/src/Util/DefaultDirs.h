#pragma once

#include <TexRPLib/WinInclude.h>
#include <Shlobj.h>

namespace TexRPCli {
	// Directory provider
	class DefaultDirs {
		public:
			// Retrive app dir
			static LPCSTR getAppDir();
			// Retrive run dir
			static LPCSTR getRunDir();
			// Retrive user dir (documents/TexRPLib)
			static LPCSTR getUserDir();

	};
}