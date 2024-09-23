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
	Reflective::AddVariable(reflectedVariables, m_loadedInMemory, "loadedInMemory", GameplayManager::GetGameState() == GameState::Stopped);
	return reflectedVariables;
}

AudioClip::AudioClip()
{
	AudioClipSettingsStandalone* audioClipSettingsStandalone = new AudioClipSettingsStandalone();
	AudioClipSettingsPSP* audioClipSettingsPSP = new AudioClipSettingsPSP();
	AudioClipSettingsPSVITA* audioClipSettingsPSVITA = new AudioClipSettingsPSVITA();

	m_settings.push_back(audioClipSettingsStandalone);
	m_settings.push_back(audioClipSettingsPSP);
	m_settings.push_back(audioClipSettingsPSVITA);
}

ReflectiveData AudioClip::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

ReflectiveData AudioClip::GetMetaReflectiveData(AssetPlatform platform)
{
	ReflectiveData reflectedVariables;
	ReflectiveData reflectedVariablesPlatform = m_settings[static_cast<int>(platform)]->GetReflectiveData();
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
	if (IsStoredInMemory() && m_fileStatus == FileStatus::FileStatus_Not_Loaded)
	{
		m_file->Open(FileMode::ReadOnly);
		m_audioMemory.m_data = (short*)m_file->ReadAllBinary(m_audioMemory.m_dataLength);
		m_file->Close();
		if (!m_audioMemory.m_data || m_audioMemory.m_dataLength == 0)
		{
			SetIsStoredInMemory(false);
			Debug::PrintError("Not enough memory for audio: " + m_file->GetPath());
		}
	}
	m_fileStatus = FileStatus::FileStatus_Loaded;
}

void AudioClip::UnloadFileReference()
{
	if (m_audioMemory.m_data)
	{
		free(m_audioMemory.m_data);
		m_audioMemory.m_data = nullptr;
	}
	m_fileStatus = FileStatus::FileStatus_Not_Loaded;
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
