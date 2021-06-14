#pragma once

#include <vector>
#include <map>
#include <string>
#include <sstream>

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

			// Retrive argument by index
			const char* getOrderedScriptArg(unsigned int index);
			// Retrive argument by name
			const char* getNamedScriptArg(const char* name);

		private:
			// Systrem argument vector
			std::vector<const char*> m_sysArgs;

			// Name of script
			const char* m_scriptName = nullptr;

			// Script arguments
			std::vector<const char*> m_luaOrdereArgs;
			std::map<std::string, std::string> m_luaNamedArgs;
	};
}