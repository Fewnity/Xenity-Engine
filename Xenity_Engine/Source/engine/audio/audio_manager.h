#pragma once
#include <engine/api.h>

#include <vector>
#include <memory>

#if defined(__PSP__)
#include <pspkernel.h>
#elif defined(__vita__)
#include <psp2/audioout.h>
#include <psp2/kernel/threadmgr.h>
#endif

#include <mutex> // std::mutex
#include "audio_source.h"

#define SOUND_FREQUENCY 44100

class AudioClip;
class AudioClipStream;
class AudioSource;

class API PlayedSound
{
public:
	~PlayedSound();
	AudioClipStream* audioClipStream = nullptr;
	std::shared_ptr<AudioSource> audioSource;
	short* buffer = nullptr;
	int seekNext = 0;
	uint64_t seekPosition = 0;
	bool needFillFirstHalfBuffer = false;
	bool needFillSecondHalfBuffer = false;

	float volume = 1;
	float pan = 0.5;
	bool loop = true;
	bool isPlaying = false;
};

class API Channel
{
public:
	Channel();
	int port = 0;

	std::vector<std::shared_ptr<PlayedSound>> playedSounds;

private:
#if defined(__vita__)
	int freq = 7;
	int mode = SCE_AUDIO_OUT_MODE_STEREO;
	int vol = SCE_AUDIO_VOLUME_0DB;
#endif
};

class API MyMutex
{
public:
	std::mutex audioMutex;
#if defined(__vita__)
	int mutexid = -1;
#endif

	/**
	* Lock mutex
	*/
	void Lock()
	{
#if defined(__vita__)
		sceKernelLockMutex(mutexid, 1, nullptr);
#else
		audioMutex.lock();
#endif
	}

	/**
	* Unlock mutex
	*/
	void Unlock()
	{
#if defined(__vita__)
		sceKernelUnlockMutex(mutexid, 1);
#else
		audioMutex.unlock();
#endif
	}
};

class API AudioManager
{
public:

	/**
	* Init audio manager
	*/
	static int Init();

	/**
	* Unload audio manager
	*/
	static void Stop();

	/**
	* Remove an audio source
	* @param audioSource Audio source
	*/
	static void RemoveAudioSource(const std::shared_ptr<AudioSource>& audioSource);

	/**
	* Play an audio source
	* @param audioSource Audio source
	*/
	static void PlayAudioSource(const std::shared_ptr<AudioSource>& audioSource);

	/**
	* Stop an audio source
	* @param audioSource Audio source
	*/
	static void StopAudioSource(const std::shared_ptr<AudioSource>& audioSource);

	static bool isAdding;
	static Channel* channel;
	static MyMutex* myMutex;

private:
};