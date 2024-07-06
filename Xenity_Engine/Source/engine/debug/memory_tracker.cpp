#include "memory_tracker.h"

MemoryTracker::MemoryTracker(const std::string& name)
{
	this->name = name;
}

void MemoryTracker::Allocate(size_t size)
{
	allocatedMemory += size;
	allocCount++;
}

void MemoryTracker::Deallocate(size_t size)
{
	deallocatedMemory += size;
	deallocCount++;
}
