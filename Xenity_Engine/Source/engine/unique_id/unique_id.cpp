// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "unique_id.h"

uint64_t UniqueId::lastUniqueId = 0;
uint64_t UniqueId::lastFileUniqueId = UniqueId::reservedFileId;

UniqueId::UniqueId()
{
	this->forFile = false;
	this->uniqueId = GenerateUniqueId(forFile);
}

UniqueId::UniqueId(bool _forFile)
{
	this->forFile = _forFile;
	this->uniqueId = GenerateUniqueId(_forFile);
}

uint64_t UniqueId::GenerateUniqueId(bool forFile)
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