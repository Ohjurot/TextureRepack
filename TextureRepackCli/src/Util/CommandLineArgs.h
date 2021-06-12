#pragma once

#include <vector>

#include <Util/DefaultDirs.h>

namespace TexRPCli {
	// Command list arguments
	class CommandLineArgs {
		public:
			// Construct from app args
			CommandLineArgs(int argc, char** argv);

			// Check if system arg is present
			bool checkSystemArg(const char* name);
			// Retrive name / path of script
			const char* getScriptName();

		private:
			// Systrem argument vector
			std::vector<const char*> m_sysArgs;

			// Name of script
			const char* m_scriptName = nullptr;
	};
}