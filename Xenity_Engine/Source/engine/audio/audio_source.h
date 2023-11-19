#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include "../component.h"
#include <memory>

class AudioClip;

class API AudioSource : public Component
{
public:
	AudioSource();
	~AudioSource();

	ReflectiveData GetReflectiveData() override;

	void Awake() override;

	/**
	* Play audio
	*/
	void Play();

	/**
	* Resume audio
	*/
	void Resume();

	/**
	* Pause audio
	*/
	void Pause();

	/**
	* Stop audio
	*/
	void Stop();

	std::shared_ptr<AudioClip> audioClip = nullptr;

	/**
	* Set volume
	*/
	void SetVolume(float _volume);

	/**
	* Set panning
	*/
	void SetPanning(float panning);

	/**
	* Set is looping
	*/
	void SetLoop(bool isLooping);

	/**
	* Get volume
	*/
	float GetVolume() const
	{
		return volume;
	}

	/**
	* Get panning
	*/
	float GetPanning() const
	{
		return pan;
	}

	/**
	* Get is playing
	*/
	bool GetIsPlaying() const
	{
		return isPlaying;
	}

	/**
	* Get is looping
	*/
	bool GetIsLooping() const
	{
		return loop;
	}

	void OnDrawGizmos() override;

private:
	std::shared_ptr<AudioSource> GetThisShared()
	{
		return std::dynamic_pointer_cast<AudioSource>(shared_from_this());
	}

	float volume = 1;
	float pan = 0.5;
	bool loop = true;
	bool isPlaying = false;
	bool playOnAwake = true;
};