// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "unique_id.h"

#include <engine/asset_management/project_manager.h>
#include <engine/tools/gameplay_utility.h>
#include <engine/scene_management/scene_manager.h>

std::random_device UniqueId::rd;  // a seed source for the random number engine
std::mt19937_64 UniqueId::gen = std::mt19937_64(rd()); // mersenne_twister_engine seeded with rd()
std::uniform_int_distribution<uint64_t> UniqueId::distrib = std::uniform_int_distribution<uint64_t>(reservedFileId, std::numeric_limits<uint64_t>::max());

uint64_t UniqueId::GenerateUniqueId(bool forFile)
{
	bool needNewId = true;
	uint64_t newId;
	while (needNewId)
	{
		newId = distrib(gen);
		needNewId = false;

		// Check if the id is already used when in editor to keep a clean project/scene (it's not very a problem in game)
#if defined(EDITOR)
		if (forFile)
		{
			const std::shared_ptr<FileReference> fileReference = ProjectManager::GetFileReferenceById(newId);
			if (fileReference)
			{
				needNewId = true;
			}
		}
		else
		{
			const std::shared_ptr<GameObject> gameObject = SceneManager::FindGameObjectByIdAdvanced(newId, false);
			if (gameObject)
			{
				needNewId = true;
			}
		}
#endif
	}

	return newId;
}