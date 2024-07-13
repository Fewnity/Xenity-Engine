// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

#include "text_manager.h"

#include <engine/file_system/file_reference.h>
#include <engine/reflection/reflection.h>

#include <memory>

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

	inline std::shared_ptr <Texture>& GetFontAtlas()
	{
		return fontAtlas;
	}

	std::shared_ptr <Texture> fontAtlas = nullptr;
	bool CreateFont(const std::shared_ptr<Font>& font, const std::string& filePath);
};
