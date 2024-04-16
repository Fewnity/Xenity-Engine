#pragma once
#include <cstdint>

class UniqueId
{
public:
	UniqueId();
	UniqueId(bool _forFile);

	/**
	* Get unique Id
	*/
	uint64_t GetUniqueId() const
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
	uint64_t uniqueId;
	bool forFile = false;
};

