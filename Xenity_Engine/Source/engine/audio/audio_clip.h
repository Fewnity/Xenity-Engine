// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

#include <memory>

#include <engine/file_system/file_reference.h>
#include <engine/reflection/reflection.h>
#include <engine/application.h>

class AudioClipSettings : public Reflective
{
public:
	bool m_loadedInMemory = false;

	ReflectiveData GetReflectiveData() override;
};

class AudioClipSettingsStandalone : public AudioClipSettings
{
public:
};

class AudioClipSettingsPSVITA : public AudioClipSettings
{
public:
};

class AudioClipSettingsPSP : public AudioClipSettings
{
public:
};

class API AudioClip : public FileReference, public Reflective
{
public:
	AudioClip();

protected:
	friend class AudioClipStream;
	friend class ProjectManager;

	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData(AssetPlatform platform) override;
	static std::shared_ptr<AudioClip> MakeAudioClip();

	void LoadFileReference() override;
	void UnloadFileReference() override;
	void OnReflectionUpdated() override;

	// Struct that stores the full audio data if the clip is stored in memory
	struct AudioMemory
	{
		int m_dataLength = 0;
		short* m_data = nullptr;
	};

	std::vector<AudioClipSettings*> m_settings;

	/**
	* [Internal] Is the audio clip stored in memory?
	*/
	inline bool IsStoredInMemory() const
	{
		return m_settings[static_cast<int>(Application::GetAssetPlatform())]->m_loadedInMemory;
	}

	inline void SetIsStoredInMemory(bool value) const
	{
		m_settings[static_cast<int>(Application::GetAssetPlatform())]->m_loadedInMemory = value;
	}

	/**
	* [Internal] Get the audio memory data (to use if the audio is stored in memory)
	*/
	inline const AudioMemory& GetAudioMemory() const
	{
		return m_audioMemory;
	}

	AudioMemory m_audioMemory;
};