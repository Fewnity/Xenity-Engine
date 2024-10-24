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
#include <engine/debug/stack_debug_object.h>

ReflectiveData AudioClipSettings::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, m_loadedInMemory, "loadedInMemory", GameplayManager::GetGameState() == GameState::Stopped);
	return reflectedVariables;
}

AudioClip::AudioClip()
{
	// Create platform specific settings
	std::unique_ptr<AudioClipSettings> settingsStandalone = std::make_unique<AudioClipSettingsStandalone>();
	std::unique_ptr<AudioClipSettings> settingsPSP = std::make_unique<AudioClipSettingsPSP>();
	std::unique_ptr<AudioClipSettings> settingsPSVITA = std::make_unique<AudioClipSettingsPSVITA>();
	std::unique_ptr<AudioClipSettings> settingsPS3 = std::make_unique<AudioClipSettingsPS3>();

	m_settings[AssetPlatform::AP_Standalone] = std::move(settingsStandalone);
	m_settings[AssetPlatform::AP_PSP] = std::move(settingsPSP);
	m_settings[AssetPlatform::AP_PsVita] = std::move(settingsPSVITA);
	m_settings[AssetPlatform::AP_PS3] = std::move(settingsPS3);
}

ReflectiveData AudioClip::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

ReflectiveData AudioClip::GetMetaReflectiveData(AssetPlatform platform)
{
	ReflectiveData reflectedVariables;
	// Add platform specific settings variables to the list of reflected variables
	ReflectiveData reflectedVariablesPlatform = m_settings[platform]->GetReflectiveData();
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
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	if (IsStoredInMemory() && m_fileStatus == FileStatus::FileStatus_Not_Loaded)
	{
		if (m_file->Open(FileMode::ReadOnly))
		{
			m_audioMemory.m_data = reinterpret_cast<short*>(m_file->ReadAllBinary(m_audioMemory.m_dataLength));
			m_file->Close();
			if (!m_audioMemory.m_data || m_audioMemory.m_dataLength == 0)
			{
				SetIsStoredInMemory(false);
				Debug::PrintError("[AudioClip::LoadFileReference] Not enough memory for audio: " + m_file->GetPath());
			}
		}
		else 
		{
			Debug::PrintError("[AudioClip::LoadFileReference] Failed to open audio clip file");
		}
	}
	m_fileStatus = FileStatus::FileStatus_Loaded;
}

void AudioClip::UnloadFileReference()
{
	// If audio file was loaded in memory, free data
	if (m_audioMemory.m_data)
	{
		free(m_audioMemory.m_data);
		m_audioMemory.m_data = nullptr;
	}
	m_fileStatus = FileStatus::FileStatus_Not_Loaded;
}

void AudioClip::OnReflectionUpdated()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	// Reload file
	if (GameplayManager::GetGameState() == GameState::Stopped)
	{
		UnloadFileReference();
		LoadFileReference();
	}
}
