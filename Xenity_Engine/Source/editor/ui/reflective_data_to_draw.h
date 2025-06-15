#pragma once

#include <vector>
#include <string>
#include <memory>

#include <engine/reflection/reflection.h>
#include <engine/platform.h>

class Command;

struct ReflectiveDataToDraw
{
	enum class OwnerTypeEnum
	{
		None = -1,
		FileReference = 0,
		GameObject = 1,
		Component = 2
	};

	std::vector<ReflectiveEntry> entryStack;
	std::vector<ReflectiveData> reflectiveDataStack;
	ReflectiveEntry currentEntry;
	std::string name;
	std::shared_ptr<Command> command;
	uint64_t ownerUniqueId = 0;
	OwnerTypeEnum ownerType = OwnerTypeEnum::None; // The owner type is used to find the object that this data belongs to later on from the ownerUniqueId.
	bool isMeta = false;
	AssetPlatform platform = AssetPlatform::AP_Standalone;
};