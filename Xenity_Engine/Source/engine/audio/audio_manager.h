// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal] Classes not visible by the user
 */

#include <vector>
#include <memory>

#if defined(__PSP__)
#include <pspkernel.h>
#include <pspthreadman.h>
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

class PlayedSound
{
public:
	~PlayedSound();
	uint64_t seekPosition = 0;
	AudioClipStream* audioClipStream = nullptr;
	std::weak_ptr<AudioSource> audioSource;
	short* buffer = nullptr;
	int seekNext = 0;
	float volume = 1;
	float pan = 0.5;
	bool needFillFirstHalfBuffer = false;
	bool needFillSecondHalfBuffer = false;

	bool loop = true;
	bool isPlaying = false;
};

class Channel
{
public:
	Channel();
	int port = 0;

	std::vector<PlayedSound*> playedSounds;
	int playedSoundsCount = 0;

private:
#if defined(__vita__)
	int freq = 7;
	int mode = SCE_AUDIO_OUT_MODE_STEREO;
	int vol = SCE_AUDIO_VOLUME_0DB;
#endif
};

class MyMutex
{
public:
	MyMutex() = delete;
	MyMutex(const std::string& mutexName);

#if defined(__vita__)
	int mutexid = -1;
	//#elif defined(__PSP__)
	//SceLwMutexWorkarea workarea;
#else
	std::mutex audioMutex;
#endif

	/**
	* @brief Lock mutex
	*/
	inline void Lock()
	{
#if defined(__vita__)
		sceKernelLockMutex(mutexid, 1, nullptr);
//#elif defined(__PSP__)
//		sceKernelLockLwMutex(&workarea, 1, nullptr);
#else
		audioMutex.lock();
#endif
	}

	/**
	* @brief Unlock mutex
	*/
	inline void Unlock()
	{
#if defined(__vita__)
		sceKernelUnlockMutex(mutexid, 1);
//#elif defined(__PSP__)
//		sceKernelUnlockLwMutex(&workarea, 1);
#else
		audioMutex.unlock();
#endif
	}
};

class AudioManager
{
public:

	/**
	* @brief Init audio manager
	*/
	static int Init();

	/**
	* @brief Stop audio manager
	*/
	static void Stop();

	/**
	* @brief Remove an audio source
	* @param audioSource Audio source
	*/
	static void RemoveAudioSource(AudioSource* audioSource);

	/**
	* @brief Play an audio source
	* @param audioSource Audio source
	*/
	static void PlayAudioSource(const std::shared_ptr<AudioSource>& audioSource);


	/**
	* @brief Stop an audio source
	* @param audioSource Audio source
	*/
	static void StopAudioSource(const std::shared_ptr<AudioSource>& audioSource);

	static bool isAdding;
	static Channel* channel;
	static MyMutex* myMutex;

private:
};