// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "../unit_test_manager.h"

#include <engine/debug/debug.h>
#include <engine/unique_id/unique_id.h>

TestResult UniqueIdTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	const UniqueId id = UniqueId();
	const UniqueId id2 = UniqueId();
	EXPECT_NOT_EQUALS(id.GetUniqueId(), id2.GetUniqueId(), "Bad UniqueId generation");

	const UniqueId idFile = UniqueId(true);
	const UniqueId idFile2 = UniqueId(true);
	EXPECT_NOT_EQUALS(idFile.GetUniqueId(), idFile2.GetUniqueId(), "Bad UniqueId generation for file");

	EXPECT_FALSE(idFile.GetUniqueId() < UniqueId::reservedFileId, "Bad UniqueId generation, reservedId not respected");
	EXPECT_FALSE(idFile2.GetUniqueId() < UniqueId::reservedFileId, "Bad UniqueId generation, reservedId not respected");

	END_TEST();
}