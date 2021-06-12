#include "FileFinder.h"

TexRPCli::FileFinder::FileFinder(const char* pathFile) {
	// Build filename
	std::stringstream ss;
	ss << DefaultDirs::getAppDir() << "\\" << pathFile;

	// Open file
	std::string fileContent = "${NAME}";
	HANDLE hFile = CreateFileA(ss.str().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		// Reserve buffer
		fileContent = "";
		DWORD size = GetFileSize(hFile, NULL);
		fileContent.reserve(size + 1);

		// Read file
		ReadFile(hFile, fileContent.data(), size, NULL, NULL);
		fileContent.data()[size] = '\0';
	
		// Close the file
		CloseHandle(hFile);
	}

	// Parse lines
	const char* readHead = fileContent.data();
	const char* readEnd = &fileContent.data()[strlen(readHead)];
	int state = 0;
	// For each character
	ss.str("");
	while (readHead != readEnd) {
		switch (state) {
			// Normal string
			case 0:
				if (*readHead == '\r' || *readHead == '\n') {
					// Next state
					state = 1;
					// Push back
					m_paths.push_back(ss.str());
					ss.str("");
				}
				else {
					// Append string
					ss << *readHead;
				}
				break;
			// Line break
			case 1:
				if (*readHead != '\r' && *readHead != '\n') {
					// Append string
					ss << *readHead;
					
					// Back to new string
					state = 0;
				}
				break;
		}

		// Increment
		readHead++;
	}
	// Append last string
	auto tempString = ss.str();
	ss.str("");
	if (tempString.length()) {
		m_paths.push_back(tempString);
	}
}

bool TexRPCli::FileFinder::findFile(const char* fileName, char* ptrBuffer, size_t bufferLen) {
	// For each path 
	for (auto it = m_paths.begin(); it != m_paths.end(); it++) {
		// Retrive copy of path
		std::stringstream ss, nss;

		// "${" + NAME + "}"
		// State machine arguments
		const char* readHead = it->data();
		const char* readEnd = &readHead[strlen(readHead)];
		int state = 0;

		// Itterate over string
		while (readHead != readEnd) {
			// State machine
			switch (state) {
				// State 0 - Normal string
				case 0: {
					// Check for magic char
					if (*readHead == '$') {
						// Check if is expression
						if (readHead + 1 < readEnd && readHead[1] == '{') {
							readHead++;
							state = 1;
						}
					}
					else {
						// Append to final string
						ss << *readHead;
					}
					break;
				}

				// State 1 - Name of replace magic
				case 1: {
					// Check for end of replace expression
					if (*readHead == '}') {
						// Check name
						std::string replaceName = nss.str();
						if (replaceName == "APP_DIR") {
							ss << DefaultDirs::getAppDir();
						}
						else if (replaceName == "RUN_DIR") {
							ss << DefaultDirs::getRunDir();
						}
						else if (replaceName == "USR_DIR") {
							ss << DefaultDirs::getUserDir();
						}
						else if (replaceName == "NAME") {
							ss << fileName;
						}

						// Go back to state 0
						state = 0;
						nss.str("");
					}
					else {
						// Append to name
						nss << *readHead;
					}

					break;
				}
			}

			// Incrment
			readHead++;
		}

		// Check if file exists
		std::string path = ss.str();
		HANDLE hFile = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
		if (hFile != INVALID_HANDLE_VALUE) {
			// Close file
			CloseHandle(hFile);

			// Copy path
			strcpy_s(ptrBuffer, bufferLen, path.c_str());
			return true;
		}
	}
	
	// Fallback
	return false;
}
