// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <memory>

#include <engine/api.h>
#include <engine/component.h>

class AudioClip;

class API AudioSource : public Component
{
public:
	AudioSource();
	~AudioSource();

	/**
	* @brief Play audio
	*/
	void Play();

	/**
	* @brief Resume audio
	*/
	void Resume();

	/**
	* @brief Pause audio
	*/
	void Pause();

	/**
	* @brief Stop audio
	*/
	void Stop();

	/**
	* @brief Set volume
	* @param _volume
	*/
	void SetVolume(float _volume);

	/**
	* @brief Set panning
	* @param panning
	*/
	void SetPanning(float panning);

	/**
	* @brief Set is looping
	* @param isLooping
	*/
	void SetLoop(bool isLooping);

	/**
	* @brief Get volume
	*/
	inline float GetVolume() const
	{
		return volume;
	}

	/**
	* @brief Get panning
	*/
	inline float GetPanning() const
	{
		return pan;
	}

	/**
	* @brief Get is playing
	*/
	inline bool IsPlaying() const
	{
		return isPlaying;
	}

	/**
	* @brief Get is looping
	*/
	inline bool IsLooping() const
	{
		return loop;
	}

	inline std::shared_ptr<AudioClip> GetAudioClip()
	{
		return audioClip;
	}

	inline void SetAudioClip(std::shared_ptr<AudioClip> audioClip)
	{
		this->audioClip = audioClip;
	}

protected:
	friend class Editor;
	friend class AudioManager;

	void OnDrawGizmos() override;

	void RemoveReferences() override;

	ReflectiveData GetReflectiveData() override;

	void Awake() override;

	std::shared_ptr<AudioClip> audioClip = nullptr;

	/**
	* @brief Get shared pointer from this
	*/
	inline std::shared_ptr<AudioSource> GetThisShared()
	{
		return std::dynamic_pointer_cast<AudioSource>(shared_from_this());
	}

	float volume = 1;
	float pan = 0.5f;
	bool loop = true;
	bool isPlaying = false;
	bool playOnAwake = true;

	// [Internal]
	bool isEditor = false;
};