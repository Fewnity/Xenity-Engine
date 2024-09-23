#pragma once

#include <string>

class MemoryTracker
{
public:
	MemoryTracker(const std::string& name);

	void Allocate(size_t size);
	void Deallocate(size_t size);

	std::string m_name;

	size_t m_allocatedMemory = 0;
	size_t m_deallocatedMemory = 0;
	size_t m_allocCount = 0;
	size_t m_deallocCount = 0;
};

