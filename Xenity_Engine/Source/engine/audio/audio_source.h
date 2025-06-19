// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <memory>

#include <engine/api.h>
#include <engine/component.h>

class AudioClip;

/**
* @brief Component to play audio clips
*/
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
	void SetVolume(float volume);

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
	[[nodiscard]] float GetVolume() const
	{
		return m_volume;
	}

	/**
	* @brief Get panning
	*/
	[[nodiscard]] float GetPanning() const
	{
		return m_pan;
	}

	/**
	* @brief Get is playing
	*/
	[[nodiscard]] bool IsPlaying() const
	{
		return m_isPlaying;
	}

	/**
	* @brief Get is looping
	*/
	[[nodiscard]] bool IsLooping() const
	{
		return m_loop;
	}

	/**
	* @brief Get audio clip
	*/
	[[nodiscard]] const std::shared_ptr<AudioClip>& GetAudioClip()
	{
		return m_audioClip;
	}

	/**
	* @brief Set audio clip
	*/
	void SetAudioClip(const std::shared_ptr<AudioClip>& audioClip)
	{
		m_audioClip = audioClip;
	}

protected:
	friend class Editor;
	friend class AudioManager;

	void OnDrawGizmos() override;

	void RemoveReferences() override;

	[[nodiscard]] ReflectiveData GetReflectiveData() override;

	void Awake() override;

	std::shared_ptr<AudioClip> m_audioClip = nullptr;

	/**
	* @brief Get shared pointer from this
	*/
	[[nodiscard]] std::shared_ptr<AudioSource> GetThisShared()
	{
		return std::dynamic_pointer_cast<AudioSource>(shared_from_this());
	}

	float m_volume = 1;
	float m_pan = 0.5f;
	bool m_loop = true;
	bool m_isPlaying = false;
	bool m_playOnAwake = true;

	// [Internal]
	bool m_isEditor = false;
};