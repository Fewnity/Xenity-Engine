// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#if defined(__PSP__)

#include <vector>
#include <engine/api.h>
#include <engine/file_system/file_reference.h>
#include <engine/reflection/reflection.h>
#include <engine/graphics/2d_graphics/sprite_selection.h>
#include <engine/reflection/enum_utils.h>
#include <engine/platform.h>
#include <engine/application.h>
#include <engine/graphics/texture.h>

/**
* @brief Texture file class
*/
class API TexturePSP : public Texture
{
public:
	TexturePSP();
	~TexturePSP();

protected:
	void OnLoadFileReferenceFinished() override;

	/**
	 * @brief Create texture data for a mipmap level
	 * @param level Mipmap level
	 * @param texData Base texture data
	 */
	void SetTextureLevel(int level, const unsigned char* texData);
	void SetData(const unsigned char* data) override;
};

#endif