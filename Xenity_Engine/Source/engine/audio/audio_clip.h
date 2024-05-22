// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
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
	struct AudioMemory
	{
		int dataLength = 0;
		short* data = nullptr;
	};
	AudioClip();

	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData() override;
	static std::shared_ptr<AudioClip> MakeAudioClip();

	void LoadFileReference() override;
	void UnloadFileReference() override;

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

private:
	AudioMemory audioMemory;
	bool loadedInMemory = false;
};