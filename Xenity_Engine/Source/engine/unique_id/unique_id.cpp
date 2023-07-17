#include "unique_id.h"
#include "../engine.h"

uint64_t UniqueId::lastUniqueId;

UniqueId::UniqueId()
{
	uniqueId = GenerateUniqueId();
}

uint64_t UniqueId::GenerateUniqueId()
{
	lastUniqueId++;
	return lastUniqueId;
}