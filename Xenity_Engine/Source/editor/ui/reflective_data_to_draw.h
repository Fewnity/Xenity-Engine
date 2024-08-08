#pragma once

#include <vector>
#include <string>
#include <memory>

#include <engine/reflection/reflection.h>
#include <engine/platform.h>

class Command;

struct ReflectiveDataToDraw
{
	std::vector<ReflectiveEntry> entryStack;
	std::vector<ReflectiveData> reflectiveDataStack;
	ReflectiveEntry currentEntry;
	std::string name;
	std::shared_ptr<Command> command;
	uint64_t ownerUniqueId = 0;
	int ownerType = -1; // -1 no parent, 0 FileReference, 1 GameObject, 2 Component
	bool isMeta = false;
	AssetPlatform platform = AssetPlatform::AP_Standalone;
};