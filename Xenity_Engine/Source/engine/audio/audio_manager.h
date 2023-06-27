#pragma once

#include <vector>
#include <memory>

#ifdef __PSP__
#include <pspkernel.h>
#endif
#ifdef __vita__
#include <psp2/audioout.h>
#include <psp2/kernel/threadmgr.h>
#endif

#include <mutex> // std::mutex
#include "../audio/audio_source.h"

#define SOUND_FREQUENCY 44100

class AudioClip;
class AudioClipStream;
class AudioSource;

class PlayedSound
{
public:
    ~PlayedSound();
    AudioClipStream *audioClipStream = nullptr;
    std::shared_ptr<AudioSource> audioSource;
    short *buffer = nullptr;
    int seekNext = 0;
    int seekPosition = 0;
    bool needNewRead = false;
    bool needNewRead2 = false;

    float volume = 1;
    float pan = 0.5;
    bool loop = true;
    bool isPlaying = false;
};

class Channel
{
public:
    Channel();
    int port;

    std::vector<std::shared_ptr<PlayedSound>> playedSounds;

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
    std::mutex audioMutex;
    int mutexid;
    void Lock()
    {
#if defined(__vita__)
        sceKernelLockMutex(mutexid, 1, nullptr);
#else
        audioMutex.lock();
#endif
    }

    void Unlock()
    {
#if defined(__vita__)
        sceKernelUnlockMutex(mutexid, 1);
#else
        audioMutex.unlock();
#endif
    }
};

class AudioManager
{
public:
    static void Init();
    static std::vector<Channel *> channels;
    static int channelCount;
    static void RemoveAudioSource(std::weak_ptr<AudioSource> audioSource);

    static void Update();
    static void PlayAudioSource(std::weak_ptr<AudioSource> audioSource);
    static void StopAudioSource(std::weak_ptr<AudioSource> audioSource);
    static bool isAdding;
    static MyMutex *myMutex;

private:
};