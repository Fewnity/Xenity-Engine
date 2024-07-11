#include "memory_tracker.h"

#include <engine/assertions/assertions.h>

MemoryTracker::MemoryTracker(const std::string& name)
{
	XASSERT(!name.empty(), "[MemoryTracker::MemoryTracker] Name is empty");
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
