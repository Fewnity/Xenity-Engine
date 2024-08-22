// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "unique_id.h"

uint64_t UniqueId::lastUniqueId = 0;
uint64_t UniqueId::lastFileUniqueId = UniqueId::reservedFileId;