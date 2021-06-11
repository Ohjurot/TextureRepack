#include "HandleManager.h"

TexRPCli::Lua::HandleManager TexRPCli::Lua::HandleManager::s_instance;

TexRPCli::Lua::HandleManager& TexRPCli::Lua::HandleManager::get() {
	return s_instance;
}

TexRPCli::Lua::HandleManager::~HandleManager() {
	release();
}

TexRPCli::Lua::Handle* TexRPCli::Lua::HandleManager::allocate() {
	// Handle pointer
	Handle* ptrHandle = &m_arrHandles[m_peekPos];
	unsigned int maxItterations = MAX_HANDLES;

	// Find first free handle
	while (ptrHandle->isOpen() && maxItterations) {
		// Increment peek next
		m_peekPos = (m_peekPos + 1) % MAX_HANDLES;
		ptrHandle = &m_arrHandles[m_peekPos];
		maxItterations--;
	}

	// Return handle or null
	return maxItterations != 0 ? ptrHandle : nullptr;
}

void TexRPCli::Lua::HandleManager::release() {
	// Close all handles
	unsigned int numOpenHandles = 0;
	int maxHandleLevel = 0;
	for (unsigned int i = 0; i < MAX_HANDLES; i++) {
		// Aquire handle count and max level
		if (m_arrHandles[i].isOpen()) {
			numOpenHandles++;
			maxHandleLevel = std::max(maxHandleLevel, m_arrHandles[i].handleLevel);
		}
	}

	// Levels
	while (numOpenHandles) {
		for (unsigned int i = 0; i < MAX_HANDLES; i++) {
			// Check all hanldess
			if (m_arrHandles[i].isOpen() && m_arrHandles[i].handleLevel == maxHandleLevel) {
				// Close and decrement
				m_arrHandles[i].close();
				numOpenHandles--;
			}
		}

		// Decrement handle level
		maxHandleLevel--;
	}
}
