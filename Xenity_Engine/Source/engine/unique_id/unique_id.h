// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <cstdint>
#include <engine/api.h>

/**
* @brief Class to inherit if you want an object to have an unique Id.
*/
class API UniqueId
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

	static constexpr uint64_t reservedFileId = 100000;

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

