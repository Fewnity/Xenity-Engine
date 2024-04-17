#pragma once
#include <engine/api.h>

#include <engine/component.h>
#include <memory>

class AudioClip;

class API AudioSource : public Component
{
public:
	AudioSource();
	~AudioSource();

	void RemoveReferences() override;

	ReflectiveData GetReflectiveData() override;

	void Awake() override;

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

	std::shared_ptr<AudioClip> audioClip = nullptr;

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
	float GetVolume() const
	{
		return volume;
	}

	/**
	* @brief Get panning
	*/
	float GetPanning() const
	{
		return pan;
	}

	/**
	* @brief Get is playing
	*/
	bool GetIsPlaying() const
	{
		return isPlaying;
	}

	/**
	* @brief Get is looping
	*/
	bool GetIsLooping() const
	{
		return loop;
	}

	void OnDrawGizmos() override;

private:

	/**
	* @brief Get shared pointer from this
	*/
	std::shared_ptr<AudioSource> GetThisShared()
	{
		return std::dynamic_pointer_cast<AudioSource>(shared_from_this());
	}

	float volume = 1;
	float pan = 0.5f;
	bool loop = true;
	bool isPlaying = false;
	bool playOnAwake = true;

public:
	// [Internal]
	bool isEditor = false;
};