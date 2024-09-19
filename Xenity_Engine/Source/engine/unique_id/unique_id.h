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
	inline UniqueId()
	{
		m_forFile = false;
		m_uniqueId = GenerateUniqueId(m_forFile);
	}

	inline UniqueId(bool _forFile)
	{
		m_forFile = _forFile;
		m_uniqueId = GenerateUniqueId(_forFile);
	}

	/**
	* @brief Get unique Id
	*/
	inline uint64_t GetUniqueId() const
	{
		return m_uniqueId;
	}

private:
	friend class ProjectManager;
	friend class SceneManager;
	friend class Compiler;
	friend class InspectorCreateGameObjectCommand;
	friend class InspectorDeleteGameObjectCommand;
	friend class EngineAssetManagerMenu;
	friend class Cooker;
	template<typename T>
	friend class InspectorDeleteComponentCommand;

	static uint64_t lastFileUniqueId;
	static uint64_t lastUniqueId;

	static constexpr uint64_t reservedFileId = 100000;

	/**
	* @brief [Internal] Generate a new id
	* @param forFile Is an Id for a file
	* @return new Id
	*/
	static inline uint64_t GenerateUniqueId(bool forFile)
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

	/**
	* @brief [Internal] Set unique Id
	* @param id Id to set
	*/
	inline void SetUniqueId(uint64_t id)
	{
		m_uniqueId = id;
	}

	uint64_t m_uniqueId;
	bool m_forFile = false;
};

