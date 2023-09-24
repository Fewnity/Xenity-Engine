#include "unique_id.h"
#include "../engine.h"

uint64_t UniqueId::lastUniqueId = 0;
uint64_t UniqueId::lastFileUniqueId = 0;

UniqueId::UniqueId()
{
	this->forFile = false;
	this->uniqueId = GenerateUniqueId(forFile);
}

UniqueId::UniqueId(bool _forFile)
{
	this->forFile = _forFile;
	this->uniqueId = GenerateUniqueId(_forFile);
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