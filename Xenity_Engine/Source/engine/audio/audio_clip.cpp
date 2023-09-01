#include "audio_clip.h"
#include "../asset_management/asset_manager.h"

AudioClip::AudioClip(std::string filePath)
{
    this->filePath = filePath;
}

std::unordered_map<std::string, Variable> AudioClip::GetReflection()
{
	std::unordered_map<std::string, Variable> reflectedVariables;
	reflectedVariables.insert_or_assign("fileId", fileId);
	return reflectedVariables;
}

std::unordered_map<std::string, Variable> AudioClip::GetMetaReflection()
{
	std::unordered_map<std::string, Variable> reflectedVariables;
	return reflectedVariables;
}

std::shared_ptr<AudioClip> AudioClip::MakeAudioClip(std::string filePath)
{
	std::shared_ptr<AudioClip> newFileRef = std::make_shared<AudioClip>(filePath);
	AssetManager::AddFileReference2(newFileRef);
	return newFileRef;
}