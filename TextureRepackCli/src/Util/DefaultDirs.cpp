#include "DefaultDirs.h"

LPCSTR TexRPCli::DefaultDirs::getAppDir() {
	// Static get
	static CHAR appDir[2048] = { 0 };
	if (appDir[0] == 0) {
		// Get module file name
		GetModuleFileNameA(GetModuleHandle(NULL), appDir, 2048);
		// Find and stripe path to remove texrp.exe
		CHAR* ptrLastSlash = strrchr(appDir, '\\');
		if (ptrLastSlash) {
			*ptrLastSlash = 0x00;
		}
	}
	
	// Return pointer
	return appDir;
}

LPCSTR TexRPCli::DefaultDirs::getRunDir() {
	// Static get
	static CHAR runDir[2048] = { 0 };
	if (runDir[0] == 0) {
		GetCurrentDirectoryA(2048, runDir);
	}
	
	// Return dir
	return runDir;
}

LPCSTR TexRPCli::DefaultDirs::getUserDir() {
	// Static get
	static CHAR userDir[2048] = { 0 };
	if (userDir[0] == 0) {
		// Get path
		WCHAR* folderPath = nullptr;
		SHGetKnownFolderPath(FOLDERID_Documents, NULL, NULL, &folderPath);

		// Convert and free
		if (folderPath) {
			WideCharToMultiByte(CP_UTF8, NULL, folderPath, -1, userDir, 2048, NULL, NULL);
			CoTaskMemFree(folderPath);

			// Cat TexRP
			strcat_s<2048>(userDir, "\\TexRP");
		}
	}
	
	// Return pointer
	return userDir;
}
