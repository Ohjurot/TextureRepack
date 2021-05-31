#pragma once

#include <memory>

template<typename T>
class MallocdMemory {
	public:
		// One 
		MallocdMemory() {
			m_size = sizeof(T);
			m_ptr = (T*)malloc(m_size);
		}

		// Many
		MallocdMemory(size_t count) {
			m_size = sizeof(T) * count;
			m_ptr = (T*)malloc(m_size);
		}

		// Destroy
		~MallocdMemory() {
			if (m_ptr) {
				free(m_ptr);
			}
		}

		// Get size
		size_t size() {
			return m_size;
		}

		// Check valid
		bool valid() {
			return m_ptr != nullptr;
		}

		// Pointer function
		T* ptr() {
			return m_ptr;
		}

		// Pointer operator
		T* operator->() {
			return m_ptr;
		}

		// Convert to T*
		operator T* () {
			return m_ptr;
		}

		operator bool() {
			return m_ptr != nullptr;
		}
	private:
		// Pointer to be guarded
		T* m_ptr = nullptr;
		
		// Size 
		size_t m_size = 0;
};
