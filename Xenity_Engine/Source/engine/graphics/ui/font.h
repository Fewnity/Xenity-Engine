// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <memory>

#include <engine/api.h>
#include <engine/file_system/file_reference.h>
#include <engine/reflection/reflection.h>
#include "text_manager.h"

class Texture;

class API Font : public FileReference, public Reflective
{
public:
	~Font();

protected:
	friend class TextManager;
	friend class ProjectManager;


	static std::shared_ptr<Font> MakeFont();

	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData(AssetPlatform platform) override;

	void OnReflectionUpdated() override;
	void LoadFileReference() override;

	Character* Characters[256] = {};
	float maxCharHeight = 0;

	/**
	* @brief [Internal] Get the shared pointer of this object
	*/
	inline std::shared_ptr<Font> GetThisShared()
	{
		return std::dynamic_pointer_cast<Font>(shared_from_this());
	}

	/**
	* Get font atlas texture
	*/
	inline const std::shared_ptr <Texture>& GetFontAtlas() const
	{
		return fontAtlas;
	}

	std::shared_ptr <Texture> fontAtlas = nullptr;
	bool CreateFont(Font& font);
};
