// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
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
#include <engine/graphics/texture/texture.h>

/**
* @brief Texture file class
*/
class API TexturePSP : public Texture
{
public:
	TexturePSP();
	~TexturePSP();

	std::vector<void*> data;

protected:
	void OnLoadFileReferenceFinished() override;

	/**
	 * @brief Create texture data for a mipmap level
	 * @param level Mipmap level
	 * @param texData Base texture data
	 */
	void SetTextureLevel(int level, const unsigned char* texData);
	void SetData(const unsigned char* data) override;

	void Bind() const override;
	void ApplyTextureFilters() const;
	int GetWrapModeEnum(WrapMode wrapMode) const;
	int TypeToGUPSM(PSPTextureType psm) const;
	void Unload() override;
	unsigned int GetColorByteCount(PSPTextureType psm);
	void copy_texture_data(void* dest, const void* src, int width, int height, const PSPTextureType destType, const PSPTextureType srcType);
	void swizzle_fast(uint8_t* out, const uint8_t* in, const unsigned int width, const unsigned int height);

	// One vector element for each mipmap level
	std::vector<bool> inVram;

	unsigned int pW = 0;
	unsigned int pH = 0;
};

#endif