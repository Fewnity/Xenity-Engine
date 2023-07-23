#pragma once
#include <cstdint>

class UniqueId
{
public:
	UniqueId();
	UniqueId(bool forFile);

	uint64_t GetUniqueId()
	{
		return uniqueId;
	}

	void SetUniqueId(uint64_t id)
	{
		uniqueId = id;
	}

	static uint64_t lastFileUniqueId;
	static uint64_t lastUniqueId;
	uint64_t GenerateUniqueId();
private:
	bool forFile = false;
	uint64_t uniqueId;
};

