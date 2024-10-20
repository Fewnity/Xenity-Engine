// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#if defined(__PS3__)

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
class API TexturePS3 : public Texture
{
public:
	TexturePS3();
	~TexturePS3();

protected:
	void OnLoadFileReferenceFinished() override;

	void SetData(const unsigned char* data) override;

	void Bind() const override;
	int GetWrapModeEnum(WrapMode wrapMode) const;
	void Unload() override;

	unsigned char* m_ps3buffer = nullptr;
};

#endif