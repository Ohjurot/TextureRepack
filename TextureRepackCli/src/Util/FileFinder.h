#pragma once

#include <Util/DefaultDirs.h>
#include <vector>
#include <string>
#include <sstream>

namespace TexRPCli {
	// Finds files
	class FileFinder {
		public:
			// Construct with filename to paths
			FileFinder(const char* pathFile);
			
			// Find the file and output filename
			bool findFile(const char* fileName, char* ptrBuffer, size_t bufferLen);

		private:
			// Search paths
			std::vector<std::string> m_paths;
	};
}