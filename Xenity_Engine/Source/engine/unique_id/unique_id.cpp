#include "unique_id.h"

const uint64_t UniqueId::reservedFileId = 100000;
uint64_t UniqueId::lastUniqueId = 0;
uint64_t UniqueId::lastFileUniqueId = UniqueId::reservedFileId;

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

void UniqueId::ResetIds() 
{
	lastUniqueId = 0;
	lastFileUniqueId = reservedFileId;
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