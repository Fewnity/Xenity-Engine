#pragma once
#include <cstdint>

class UniqueId
{
public:
	UniqueId();
	UniqueId(bool forFile);

	/**
	* Get unique Id
	*/
	uint64_t GetUniqueId()
	{
		return uniqueId;
	}

	/**
	* Set unique Id
	*/
	void SetUniqueId(uint64_t id)
	{
		uniqueId = id;
	}

	static uint64_t lastFileUniqueId;
	static uint64_t lastUniqueId;

	/**
	* Generate a new id
	* @param forFile Is an Id for a file
	* @return new Id
	*/
	static uint64_t GenerateUniqueId(bool forFile);
private:
	bool forFile = false;
	uint64_t uniqueId;
};

