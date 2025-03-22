// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal] Classes not visible by the user
 */

#include <vector>
#include <memory>
#include <mutex> // std::mutex

#if defined(__PSP__)
#include <pspkernel.h>
#include <pspthreadman.h>
#elif defined(__vita__)
#include <psp2/audioout.h>
#include <psp2/kernel/threadmgr.h>
#elif defined(__PS3__)
#include <sys/mutex.h>
#include <sys/thread.h>
#else
#include <thread>
#endif

#include "audio_source.h"
#include <engine/audio/audio_clip_stream.h>

class AudioClip;

class PlayedSound
{
public:
	PlayedSound();
	~PlayedSound();
	std::weak_ptr<AudioSource> m_audioSource;
	uint64_t m_bufferSeekPosition = 0;
	std::unique_ptr<AudioClipStream> m_audioClipStream = nullptr;
	uint64_t m_audioSeekPosition = 0;
	short* m_buffer = nullptr;
	uint32_t m_seekNext = 0;
	float m_volume = 1;
	float m_pan = 0.5;
	bool m_needFillFirstHalfBuffer = false;
	bool m_needFillSecondHalfBuffer = false;

	bool m_loop = true;
	bool m_isPlaying = false;
};

class Channel
{
public:
	Channel();
	int m_port = 0;

	std::vector<PlayedSound*> m_playedSounds;
	size_t m_playedSoundsCount = 0;

private:
#if defined(__vita__)
	int m_freq = 7;
	int m_mode = SCE_AUDIO_OUT_MODE_STEREO;
	int m_vol = SCE_AUDIO_VOLUME_0DB;
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
#elif defined(__PS3__)
	sys_mutex_t mutex;
	sys_mutex_attr_t mattr;
#else
	std::mutex m_audioMutex;
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
#elif defined(__PS3__)
		sysMutexLock(mutex, 0);
#else
		m_audioMutex.lock();
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
#elif defined(__PS3__)
		sysMutexUnlock(mutex);
#else
		m_audioMutex.unlock();
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

	static bool s_isAdding;
	static Channel* s_channel;
	static MyMutex* s_myMutex;

	static void FillChannelBuffer(short* buffer, uint64_t length, Channel* channel);

private:
#if defined(_WIN32) || defined(_WIN64)
	static std::thread sendAudioThread;
	static std::thread fillBufferThread;
#elif defined(__PS3__)
	static sys_ppu_thread_t sendAudioThread;
	static sys_ppu_thread_t fillBufferThread;
#elif defined(__PSP__) || defined(__vita__)
	static SceUID sendAudioThread;
	static SceUID fillBufferThread;
#endif
};