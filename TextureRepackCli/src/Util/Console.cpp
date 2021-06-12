#include "Console.h"

// Instance
TexRPCli::Console TexRPCli::Console::s_instance;

TexRPCli::Console& TexRPCli::Console::get() {
	return s_instance;
}

void TexRPCli::Console::setColorEnable(bool enable) {
	m_enableColor = enable;
}

void TexRPCli::Console::setOutputEnable(bool output) {
	m_outputEnable = output;
}

void TexRPCli::Console::setColor(ConsoleColor color) {
	// Exit when colors are not enabled
	if (!m_enableColor) {
		return;
	}
	
	// Convert to word
	WORD consoleAttribute = (WORD)color;

	// Retrive current settings
	CONSOLE_SCREEN_BUFFER_INFO info;
	if (GetConsoleScreenBufferInfo(m_hOutputHandle, &info)) {
		// Add to color
		consoleAttribute |= (info.wAttributes & 0xFF00);
	}
	
	// Set attribute
	SetConsoleTextAttribute(m_hOutputHandle, consoleAttribute);
}

void TexRPCli::Console::setTitle(LPCSTR title) {
	SetConsoleTitleA(title);
}

void TexRPCli::Console::write(LPCSTR text) {
	if (m_outputEnable) {
		WriteConsoleA(m_hOutputHandle, text, strlen(text), NULL, NULL);
	}
}

void TexRPCli::Console::writeLine(LPCSTR text) {
	write(text);
	write("\n");
}

DWORD TexRPCli::Console::readLine(CHAR* buffer, DWORD maxLength) {
	// Check output enable
	if (!m_outputEnable) {
		buffer[0] = '\0';
		return 0;
	}
	
	// Build read info
	CONSOLE_READCONSOLE_CONTROL readControle;
	readControle.nLength = sizeof(CONSOLE_READCONSOLE_CONTROL);
	readControle.nInitialChars = 0;
	readControle.dwCtrlWakeupMask = '\n';
	readControle.dwControlKeyState = NULL;

	// Call read
	DWORD readCount;
	if (ReadConsoleA(m_hInputHandle, buffer, maxLength - 1, &readCount, &readControle)) {
		buffer[readCount - 2] = '\0';
		return readCount - 2;
	}
	else {
		buffer[0] = '\0';
		return 0;
	}
}

TexRPCli::Console::Console() {
	// Retrive console handled
	m_hInputHandle = GetStdHandle(STD_INPUT_HANDLE);
	m_hOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	// Get console attributes
	CONSOLE_SCREEN_BUFFER_INFO info;
	if (GetConsoleScreenBufferInfo(m_hOutputHandle, &info)) {
		m_startAttributes = info.wAttributes;
	}
	else {
		m_startAttributes = (WORD)ConsoleColor::WHITE;
		SetConsoleTextAttribute(m_hOutputHandle, m_startAttributes);
	}

	// Set color
	setColor(ConsoleColor::WHITE);
}

TexRPCli::Console::~Console() {
	// Restore attributes
	SetConsoleTextAttribute(m_hOutputHandle, m_startAttributes);

	// Close handled
	CloseHandle(m_hInputHandle);
	CloseHandle(m_hOutputHandle);
}