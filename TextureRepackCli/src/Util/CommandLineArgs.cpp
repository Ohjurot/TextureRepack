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

	// Script arguments
	while (argc > index) {
		// Builder stream and strings
		std::stringstream ss;
		std::string argName;
		
		// State machine
		int state = 0;
		const char* readHead = argv[index];
		const char* readEnd = &readHead[strlen(readHead)];
		while (readHead < readEnd) {
			// State machine switch
			switch (state) {
				// Reading name / value
				case 0: {
					// Seperator --> name arg
					if (*readHead == '=') {
						// Store clear next
						argName = ss.str();
						ss.str("");
						state = 1;
					}
					// Normal char
					else {
						ss << *readHead;
					}
					break;
				}
				// Reading value
				case 1: {
					ss << *readHead;
					break;
				}
			}

			// Inrement
			readHead++;
		}

		// Check state
		if (state == 0) {
			// Push back normal ordered arg
			m_luaOrdereArgs.push_back(argv[index]);
		}
		else {
			// Insert named arg
			m_luaNamedArgs.emplace(std::pair<std::string, std::string>(argName, ss.str()));
		}

		// Increment
		index++;
	}
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

const char* TexRPCli::CommandLineArgs::getOrderedScriptArg(unsigned int index) {
	// Bound check
	if (index >= m_luaOrdereArgs.size()) {
		return nullptr;
	}

	// Return at position
	return m_luaOrdereArgs[index];
}

const char* TexRPCli::CommandLineArgs::getNamedScriptArg(const char* name) {
	auto itFind = m_luaNamedArgs.find(name);
	return itFind != m_luaNamedArgs.end() ? itFind->second.c_str() : nullptr;
}
