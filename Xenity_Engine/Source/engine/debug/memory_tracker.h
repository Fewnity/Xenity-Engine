#pragma once

#include <string>

class MemoryTracker
{
public:
	MemoryTracker(const std::string& name);

	void Allocate(size_t size);
	void Deallocate(size_t size);

	std::string name;

	size_t allocatedMemory = 0;
	size_t deallocatedMemory = 0;
	size_t allocCount = 0;
	size_t deallocCount = 0;
};

