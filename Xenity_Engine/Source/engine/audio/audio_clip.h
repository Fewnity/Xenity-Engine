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

	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData() override;
	static std::shared_ptr<AudioClip> MakeAudioClip();

	void LoadFileReference() override;
	void UnloadFileReference() override;


private:
	friend class AudioClipStream;

	struct AudioMemory
	{
		int dataLength = 0;
		short* data = nullptr;
	};

	/**
	* [Internal]
	*/
	bool GetIsLoadedInMemory() 
	{
		return loadedInMemory;
	}

	/**
	* [Internal]
	*/
	const AudioMemory& GetAudioMemory()
	{
		return audioMemory;
	}

	AudioMemory audioMemory;
	bool loadedInMemory = false;
};