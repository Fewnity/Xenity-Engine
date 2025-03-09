// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/reflection/enum_utils.h>

/**
 * [Internal]
 */

ENUM(FileType,
	File_Other,
	File_Audio,
	File_Mesh,
	File_Texture,
	File_Scene,
	File_Code,
	File_Header,
	File_Skybox,
	File_Font,
	File_Material,
	File_Shader,
	File_Icon,
	File_Prefab,
	COUNT,
	);
