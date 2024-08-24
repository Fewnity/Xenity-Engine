// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "audio_clip.h"

#include <engine/asset_management/asset_manager.h>
#include <engine/file_system/file.h>
#include <engine/debug/debug.h>
#include <engine/game_elements/gameplay_manager.h>

ReflectiveData AudioClipSettings::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, loadedInMemory, "loadedInMemory", GameplayManager::GetGameState() == GameState::Stopped);
	return reflectedVariables;
}

AudioClip::AudioClip()
{
	AudioClipSettingsStandalone* audioClipSettingsStandalone = new AudioClipSettingsStandalone();
	AudioClipSettingsPSP* audioClipSettingsPSP = new AudioClipSettingsPSP();
	AudioClipSettingsPSVITA* audioClipSettingsPSVITA = new AudioClipSettingsPSVITA();

	settings.push_back(audioClipSettingsStandalone);
	settings.push_back(audioClipSettingsPSP);
	settings.push_back(audioClipSettingsPSVITA);
}

ReflectiveData AudioClip::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

ReflectiveData AudioClip::GetMetaReflectiveData(AssetPlatform platform)
{
	ReflectiveData reflectedVariables;
	ReflectiveData reflectedVariablesPlatform = settings[static_cast<int>(platform)]->GetReflectiveData();
	reflectedVariables.insert(reflectedVariables.end(), reflectedVariablesPlatform.begin(), reflectedVariablesPlatform.end());
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
	if (IsStoredInMemory() && !m_isLoaded)
	{
		m_file->Open(FileMode::ReadOnly);
		audioMemory.data = (short*)m_file->ReadAllBinary(audioMemory.dataLength);
		m_file->Close();
		if (!audioMemory.data || audioMemory.dataLength == 0)
		{
			SetIsStoredInMemory(false);
			Debug::PrintError("Not enough memory for audio: " + m_file->GetPath());
		}
	}
	m_isLoaded = true;
}

void AudioClip::UnloadFileReference()
{
	if (audioMemory.data)
	{
		free(audioMemory.data);
		audioMemory.data = nullptr;
	}
	m_isLoaded = false;
}

void AudioClip::OnReflectionUpdated()
{
	// Reload file
	if (GameplayManager::GetGameState() == GameState::Stopped)
	{
		UnloadFileReference();
		LoadFileReference();
	}
}
