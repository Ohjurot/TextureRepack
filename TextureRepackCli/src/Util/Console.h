#pragma once

#include <TexRPLib/WinInclude.h>

namespace TexRPCli {
	// Console color array
	enum class ConsoleColor : WORD {
		// Black / White
		BLACK = 0,
		WHITE = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		// Single color
		RED = FOREGROUND_RED,
		GREEN = FOREGROUND_GREEN,
		BLUE = FOREGROUND_BLUE,
		// Light single color
		LIGHT_RED = FOREGROUND_RED | FOREGROUND_INTENSITY,
		LIGHT_GREEN = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
		LIGHT_BLUE = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		// Special color
		ORANGE = FOREGROUND_RED | FOREGROUND_GREEN,
	};

	// Represents the console
	class Console {
		public:
			// Static instance get function
			static TexRPCli::Console& get();

			// Set color enabled
			void setColorEnable(bool enable);
			// Set the color of the console
			void setColor(ConsoleColor color);
			// Sets the console title
			void setTitle(LPCSTR title);
			// Write text to console
			void write(LPCSTR text);
			// Write line
			void writeLine(LPCSTR text);
			// Read line
			DWORD readLine(CHAR* buffer, DWORD maxLength);

		private:
			// Private constructor
			Console();
			~Console();

			static TexRPCli::Console s_instance;
		private:
			// Windows console handled
			HANDLE m_hOutputHandle;
			HANDLE m_hInputHandle;

			// Initial console attributes
			WORD m_startAttributes;

			// Indicates 
			bool m_enableColor = true;
	};
}
