#pragma once
#include <cstdint>

class UniqueId
{
public:
	UniqueId();
	UniqueId(bool _forFile);

	/**
	* @brief Get unique Id
	*/
	uint64_t GetUniqueId() const
	{
		return uniqueId;
	}

	/**
	* @brief [Internal] Set unique Id
	* @param id Id to set
	*/
	void SetUniqueId(uint64_t id)
	{
		uniqueId = id;
	}

	static uint64_t lastFileUniqueId;
	static uint64_t lastUniqueId;

	static const uint64_t reservedFileId;

	/**
	* @brief [Internal] Generate a new id
	* @param forFile Is an Id for a file
	* @return new Id
	*/
	static uint64_t GenerateUniqueId(bool forFile);

private:

	uint64_t uniqueId;
	bool forFile = false;
};

