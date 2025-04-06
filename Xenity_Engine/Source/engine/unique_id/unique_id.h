// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <cstdint>
#include <random>

#include <engine/api.h>

/**
* @brief Class to inherit if you want an object to have an unique Id.
*/
class API UniqueId
{
public:
	inline UniqueId()
	{
		m_uniqueId = GenerateUniqueId(false);
	}

	inline UniqueId(bool _forFile)
	{
		m_uniqueId = GenerateUniqueId(_forFile);
	}

	UniqueId(const UniqueId& other) = delete;
	UniqueId& operator=(const UniqueId&) = delete;

	/**
	* @brief Get unique Id
	*/
	[[nodiscard]] inline uint64_t GetUniqueId() const
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
	friend class InspectorDeleteComponentCommand;
	friend class UniqueIdTest;
	template <class T>
	friend class SelectAssetMenu;
	friend class InspectorAddComponentCommand;

	static constexpr uint64_t reservedFileId = 100000;

	static std::random_device rd;  // a seed source for the random number engine
	static std::mt19937_64 gen; // mersenne_twister_engine seeded with rd()
	static std::uniform_int_distribution<uint64_t> distrib;

	/**
	* @brief [Internal] Generate a new id
	* @param forFile Is an Id for a file
	* @return new Id
	*/
	static uint64_t GenerateUniqueId(bool forFile);

	/**
	* @brief [Internal] Set unique Id
	* @param id Id to set
	*/
	inline void SetUniqueId(uint64_t id)
	{
		m_uniqueId = id;
	}

	uint64_t m_uniqueId;
};

