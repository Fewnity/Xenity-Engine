#include "unique_id.h"
#include "../engine.h"

uint64_t UniqueId::lastUniqueId = 0;
uint64_t UniqueId::lastFileUniqueId = 0;

UniqueId::UniqueId()
{
	this->forFile = false;
	this->uniqueId = GenerateUniqueId(forFile);
}

UniqueId::UniqueId(bool forFile)
{
	this->forFile = forFile;
	this->uniqueId = GenerateUniqueId(forFile);
}

uint64_t UniqueId::GenerateUniqueId(bool forFile)
{
	if (forFile) 
	{
		lastFileUniqueId++;
		return lastFileUniqueId;
	}
	else 
	{
		lastUniqueId++;
		return lastUniqueId;
	}
}