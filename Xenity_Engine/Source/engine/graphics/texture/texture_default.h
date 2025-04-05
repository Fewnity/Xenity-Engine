// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__) || defined(__vita__)

#include <vector>
#include <engine/api.h>
#include <engine/file_system/file_reference.h>
#include <engine/reflection/reflection.h>
#include <engine/graphics/2d_graphics/sprite_selection.h>
#include <engine/reflection/enum_utils.h>
#include <engine/platform.h>
#include <engine/application.h>
#include <engine/graphics/texture/texture.h>

/**
* @brief Texture file class
*/
class API TextureDefault : public Texture
{
public:
	TextureDefault() = default;
	~TextureDefault();

	/**
	* @brief [Internal] Get texture ID
	*/
	[[nodiscard]] unsigned int GetTextureId() const
	{
		return m_textureId;
	}

protected:
	void OnLoadFileReferenceFinished() override;

	void SetData(const unsigned char* data) override;

	void Bind() const override;
	void ApplyTextureFilters() const;
	[[nodiscard]] int GetWrapModeEnum(WrapMode wrapMode) const;
	void Unload() override;

	unsigned int m_textureId = -1;
};

#endif