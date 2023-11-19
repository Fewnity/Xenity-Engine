#include "audio_clip.h"
#include "../asset_management/asset_manager.h"

AudioClip::AudioClip()
{
}

ReflectiveData AudioClip::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

ReflectiveData AudioClip::GetMetaReflection()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

std::shared_ptr<AudioClip> AudioClip::MakeAudioClip()
{
	std::shared_ptr<AudioClip> newFileRef = std::make_shared<AudioClip>();
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
}