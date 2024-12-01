// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "performance.h"

#include <cstring>

#include <engine/time/time.h>
#include <engine/debug/debug.h>
#include <engine/engine_settings.h>
#include <engine/file_system/file_system.h>
#include <engine/file_system/file.h>
#include "memory_tracker.h"
#include <engine/debug/stack_debug_object.h>
#include <engine/tools/endian_utils.h>

int Performance::s_drawCallCount = 0;
int Performance::s_drawTriangleCount = 0;
int Performance::s_updatedMaterialCount = 0;
std::unordered_map<std::string, ProfilerCategory*> Performance::s_profilerCategories;
std::unordered_map<uint64_t, std::vector<ScopTimerResult>> Performance::s_scopProfilerList; // Hash, List
std::unordered_map<uint64_t, std::string> Performance::s_scopProfilerNames; // Hash, Name

int Performance::s_tickCount = 0;
float Performance::s_averageCoolDown = 0;
int Performance::s_lastDrawCallCount = 0;
int Performance::s_lastDrawTriangleCount = 0;

MemoryTracker* Performance::s_gameObjectMemoryTracker = nullptr;
MemoryTracker* Performance::s_meshDataMemoryTracker = nullptr;
MemoryTracker* Performance::s_textureMemoryTracker = nullptr;
uint32_t Performance::s_benchmarkScopeLevel = 0;

#pragma region Update values

void Performance::Init()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);
	Debug::Print("-------- Profiler initiated --------", true);

#if defined(DEBUG)
	s_gameObjectMemoryTracker = new MemoryTracker("GameObjects");
	s_meshDataMemoryTracker = new MemoryTracker("Mesh Data");
	s_textureMemoryTracker = new MemoryTracker("Textures");
#endif
}

void Performance::ResetCounters()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);
	s_lastDrawCallCount = s_drawCallCount;
	s_drawCallCount = 0;
	s_lastDrawTriangleCount = s_drawTriangleCount;
	s_drawTriangleCount = 0;

	s_updatedMaterialCount = 0;
	ResetProfiler();
}

void Performance::AddDrawCall()
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);
	s_drawCallCount++;
}

void Performance::AddDrawTriangles(int count)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);
	s_drawTriangleCount+= count;
}

void Performance::AddMaterialUpdate()
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);
	s_updatedMaterialCount++;
}

#pragma endregion

#pragma region Getters

int Performance::GetDrawCallCount()
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);
	return s_lastDrawCallCount;
}

int Performance::GetDrawTrianglesCount()
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);
	return s_lastDrawTriangleCount;
}

int Performance::GetUpdatedMaterialCount()
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);
	return s_updatedMaterialCount;
}

void Performance::Update()
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);
	if (EngineSettings::values.useProfiler)
	{
		s_tickCount++;

		// Update profiler average values
		s_averageCoolDown += Time::GetUnscaledDeltaTime();
		if (s_averageCoolDown >= 1)
		{
			for (const auto& categoryKV : Performance::s_profilerCategories)
			{
				for (const auto& profilerValueKV : categoryKV.second->profilerList)
				{
					profilerValueKV.second->average = profilerValueKV.second->addedValue / s_tickCount;
					profilerValueKV.second->addedValue = 0;
				}
			}
			s_averageCoolDown = 0;
			s_tickCount = 0;
		}
		ResetCounters();
	}
}

size_t Performance::RegisterScopProfiler(const std::string& name, size_t hash)
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);
	s_scopProfilerNames[hash] = name;

	return hash;
}

template<typename T>
void WriteData(std::vector<uint8_t>& buffer, T value)
{
	T newValue = value;
#if defined(__PS3__)
	newValue = EndianUtils::SwapEndian(newValue);
#endif
	buffer.insert(buffer.end(), ((uint8_t*)&newValue), ((uint8_t*)&newValue) + sizeof(newValue));
}

template<typename T>
void WriteData(std::vector<uint8_t>& buffer, T* value, size_t size)
{
	buffer.insert(buffer.end(), ((uint8_t*)value), ((uint8_t*)value) + size);
}

void Performance::SaveToBinary(const std::string& path)
{
	Debug::Print("Saving profiler data...");
	
	const std::shared_ptr<File> file = FileSystem::MakeFile(path);
	const bool isOpen = file->Open(FileMode::WriteCreateFile);
	if (isOpen) 
	{
		std::vector<uint8_t> data;
		
		// Write profiler names count
		WriteData(data, static_cast<uint32_t>(s_scopProfilerNames.size()));

		// Write profiler names
		for (const auto& profilerNamesKV : s_scopProfilerNames) 
		{
			WriteData(data, profilerNamesKV.first); // Key
			uint32_t strSize = static_cast<uint32_t>(profilerNamesKV.second.size());
			WriteData(data, strSize); // Name length
			WriteData(data, profilerNamesKV.second.data(), strSize); // Name string
		}

		// Write profiler record keys count
		WriteData(data, static_cast<uint32_t>(s_scopProfilerList.size()));

		// Write profiler records
		for (const auto& profilerRecordListKV : s_scopProfilerList)
		{
			WriteData(data, profilerRecordListKV.first); // Key

			// Write profiler records count
			WriteData(data, static_cast<uint32_t>(profilerRecordListKV.second.size()));
			
			// Write profiler record data
			for (const auto& profilerRecord : profilerRecordListKV.second)
			{
				WriteData(data, profilerRecord.start);
				WriteData(data, profilerRecord.end);
				WriteData(data, profilerRecord.level);
			}
		}

		file->Write(std::string(data.begin(), data.end()));
		file->Close();
	}
	else 
	{
		Debug::PrintError("[Performance::SaveToBinary] Failed to save profiler data");
	}
}

template<typename T>
T ReadData(unsigned char*& data)
{
	T valueToGet;
	memcpy(&valueToGet, data, sizeof(valueToGet));
	data += sizeof(valueToGet);
	return valueToGet;
}

void Performance::LoadFromBinary(const std::string& path)
{
	const std::shared_ptr<File> file = FileSystem::MakeFile(path);
	const bool isOpen = file->Open(FileMode::ReadOnly);
	if (isOpen)
	{
		// Read file
		size_t size = 0;
		unsigned char* data = file->ReadAllBinary(size);
		unsigned char* originalDataPtr = data;

		ResetProfiler();
		s_scopProfilerNames.clear();

		// Extract data

		// Read profiler names count
		uint32_t profilerNameCount = ReadData<uint32_t>(data);
		for (size_t i = 0; i < profilerNameCount; i++)
		{
			uint64_t key = ReadData<uint64_t>(data);
			uint32_t strSize = ReadData<uint32_t>(data);
			std::string str = std::string(data, data + strSize);
			data += strSize;

			s_scopProfilerNames[key] = str;
		}

		// Read profiler record keys count
		uint32_t profilerRecordKeyCount = ReadData<uint32_t>(data);

		for (size_t i = 0; i < profilerRecordKeyCount; i++)
		{
			uint64_t key = ReadData<uint64_t>(data);

			uint32_t profilerRecordCount = ReadData<uint32_t>(data);
			std::vector<ScopTimerResult> scopTimerResultList;
			for (size_t j = 0; j < profilerRecordCount; j++)
			{
				uint32_t startValue = ReadData<uint64_t>(data);
				uint32_t endValue = ReadData<uint64_t>(data);
				uint32_t levelValue = ReadData<uint32_t>(data);

				ScopTimerResult result;
				result.start = startValue;
				result.end = endValue;
				result.level = levelValue;

				scopTimerResultList.push_back(result);
			}
			s_scopProfilerList[key] = scopTimerResultList;
		}

		free(originalDataPtr);
	}
	else
	{
		Debug::PrintError("[Performance::LoadFromBinary] Failed to load profiler data");
	}
}

void Performance::ResetProfiler()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	s_scopProfilerList.clear();

	for (const auto& categoryKV : Performance::s_profilerCategories)
	{
		for (const auto& profilerValueKV : categoryKV.second->profilerList)
		{
			profilerValueKV.second->ResetValue();
		}
	}
}

#pragma endregion
