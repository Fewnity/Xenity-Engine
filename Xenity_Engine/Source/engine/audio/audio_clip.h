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

class API AudioClip : public FileReference, public Reflective
{
public:
	AudioClip();

protected:
	friend class AudioClipStream;
	friend class ProjectManager;

	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData() override;
	static std::shared_ptr<AudioClip> MakeAudioClip();

	void LoadFileReference() override;
	void UnloadFileReference() override;

	// Struct that stores the full audio data if the clip is stored in memory
	struct AudioMemory
	{
		int dataLength = 0;
		short* data = nullptr;
	};

	/**
	* [Internal] Is the audio clip stored in memory?
	*/
	inline bool IsStoredInMemory()
	{
		return loadedInMemory;
	}

	/**
	* [Internal] Get the audio memory data (to use if the audio is stored in memory)
	*/
	inline const AudioMemory& GetAudioMemory()
	{
		return audioMemory;
	}

	AudioMemory audioMemory;
	bool loadedInMemory = false;
};