// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "../unit_test_manager.h"
#include <engine/debug/debug.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/lighting/lighting.h>
#include <engine/game_elements/gameobject.h>
#include <engine/tools/gameplay_utility.h>
#include <engine/game_elements/gameplay_manager.h>

bool UniqueIdTest::Start(std::string& errorOut)
{
	bool result = true;

	UniqueId id = UniqueId();
	UniqueId id2 = UniqueId();
	if (Compare(id.GetUniqueId(), id2.GetUniqueId()))
	{
		errorOut += "Bad UniqueId generation\n";
		result = false;
	}

	UniqueId idFile = UniqueId(true);
	UniqueId idFile2 = UniqueId(true);
	if (Compare(idFile.GetUniqueId(), idFile2.GetUniqueId()))
	{
		errorOut += "Bad UniqueId generation for file\n";
		result = false;
	}

	if (idFile.GetUniqueId() < UniqueId::reservedFileId || idFile2.GetUniqueId() < UniqueId::reservedFileId)
	{
		errorOut += "Bad UniqueId generation for file, reservedFileId not respected\n";
		result = false;
	}

	return result;
}