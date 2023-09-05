#include "audio_clip.h"
#include "../asset_management/asset_manager.h"

AudioClip::AudioClip(std::string filePath)
{
    this->filePath = filePath;
}

std::unordered_map<std::string, ReflectionEntry> AudioClip::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	Reflection::AddReflectionVariable(reflectedVariables, fileId, "fileId", true);
	return reflectedVariables;
}

std::unordered_map<std::string, ReflectionEntry> AudioClip::GetMetaReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	return reflectedVariables;
}

std::shared_ptr<AudioClip> AudioClip::MakeAudioClip(std::string filePath)
{
	std::shared_ptr<AudioClip> newFileRef = std::make_shared<AudioClip>(filePath);
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
}