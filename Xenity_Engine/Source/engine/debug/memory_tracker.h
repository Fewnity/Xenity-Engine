#pragma once

#include <string>

class MemoryTracker
{
public:
	MemoryTracker(const std::string& name);

	void Allocate(size_t size);
	void Deallocate(size_t size);

	std::string name;

	int allocatedMemory = 0;
	int deallocatedMemory = 0;
	int allocCount = 0;
	int deallocCount = 0;
};

