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
class AudioSource;

class PlayedSound
{
public:
    AudioClip *audioClip = nullptr;
    AudioSource safeAudioSource;
    std::weak_ptr<AudioSource> audioSource;
    short *buffer = nullptr;
    int seekNext = 0;
    int seekPosition = 0;
    bool needNewRead = false;
    bool needNewRead2 = false;
};

class Channel
{
public:
    Channel();
    int port;

    std::vector<PlayedSound *> playedSounds;

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
};

class AudioManager
{
public:
    static void Init();
    static std::vector<Channel *> channels;
    static int channelCount;
    static void RemoveAudioSource(std::weak_ptr<AudioSource> audioSource);
    static void AddAudioSource(std::weak_ptr<AudioSource> audioSource);
    static void UpdateAudioSource(std::weak_ptr<AudioSource> audioSource);
    static bool isAdding;
    static MyMutex *myMutex;

private:
};