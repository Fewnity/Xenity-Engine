#include "audio_clip.h"
#include <engine/asset_management/asset_manager.h>
#include <engine/file_system/file_system.h>
#include <engine/file_system/file.h>
#include <engine/debug/debug.h>

AudioClip::AudioClip()
{
}

ReflectiveData AudioClip::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

ReflectiveData AudioClip::GetMetaReflectiveData()
{
	ReflectiveData reflectedVariables;
	AddVariable(reflectedVariables, loadedInMemory, "loadedInMemory", true);
	return reflectedVariables;
}

std::shared_ptr<AudioClip> AudioClip::MakeAudioClip()
{
	std::shared_ptr<AudioClip> newFileRef = std::make_shared<AudioClip>();
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
}

void AudioClip::LoadFileReference()
{
	if (loadedInMemory && !isLoaded) 
	{
		file->Open(FileMode::ReadOnly);
		data = (short*)file->ReadAllBinary(length);
		file->Close();
		if(!data)
		{
			loadedInMemory = false;
			Debug::PrintError("Not enough memory for audio: " + file->GetPath());
		}
		isLoaded = true;
	}
}

void AudioClip::UnloadFileReference()
{
	if (loadedInMemory && isLoaded)
	{
		free(data);
	}
	isLoaded = false;
}
