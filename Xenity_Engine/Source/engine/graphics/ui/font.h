#pragma once

#include <memory>
#include "../../file_system/file_reference.h"
#include "../../reflection/reflection.h"
#include "text_manager.h"

class Texture;

class API Font : public FileReference, public Reflection
{
public:
	~Font();
	std::shared_ptr <Texture> fontAtlas = nullptr;
	static std::shared_ptr<Font> MakeFont();

	std::unordered_map<std::string, ReflectionEntry> GetReflection() override;
	std::unordered_map<std::string, ReflectionEntry> GetMetaReflection() override;

	void OnReflectionUpdated() override;
	void LoadFileReference() override;
	Character* Characters[256] = {};
	float maxCharHeight = 0;

	std::shared_ptr<Font> GetThisShared()
	{
		return std::dynamic_pointer_cast<Font>(shared_from_this());
	}

private:
	bool CreateFont(const std::shared_ptr<Font>& font, const std::string& filePath);
};
