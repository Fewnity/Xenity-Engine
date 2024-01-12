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
	std::shared_ptr <Texture> fontAtlas = nullptr;
	static std::shared_ptr<Font> MakeFont();

	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData() override;

	void OnReflectionUpdated() override;
	void LoadFileReference() override;

	Character* Characters[256] = {};
	float maxCharHeight = 0;

	// [Internal]
	std::shared_ptr<Font> GetThisShared()
	{
		return std::dynamic_pointer_cast<Font>(shared_from_this());
	}

private:
	bool CreateFont(const std::shared_ptr<Font>& font, const std::string& filePath);
};
