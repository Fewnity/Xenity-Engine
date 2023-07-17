#pragma once
#include <cstdint>

class UniqueId
{
public:
	UniqueId();

	uint64_t GetUniqueId()
	{
		return uniqueId;
	}

	void SetUniqueId(uint64_t id)
	{
		uniqueId = id;
	}

private:
	uint64_t uniqueId;
	static uint64_t lastUniqueId;
	static uint64_t GenerateUniqueId();
};

