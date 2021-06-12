#include "CommandLineArgs.h"

TexRPCli::CommandLineArgs::CommandLineArgs(int argc, char** argv) {
	// Argc index
	int index = 1;

	// Check sys args
	while (argc > index && argv[index][0] == '-') {
		// Store sysarg
		m_sysArgs.push_back(&argv[index][1]);
		// Increment
		index++;
	}

	// Check if stil valid
	if (argc > index) {
		m_scriptName = argv[index];
		index++;
	}

	// TODO: Script arguments
}

bool TexRPCli::CommandLineArgs::checkSystemArg(const char* name) {
	// Itteratr over vector
	for (auto it = m_sysArgs.begin(); it != m_sysArgs.end(); it++) {
		if (strcmp(name, *it) == 0) {
			return true;
		}
	}
	
	// Fallback
	return false;
}

const char* TexRPCli::CommandLineArgs::getScriptName() {
	return m_scriptName;
}
