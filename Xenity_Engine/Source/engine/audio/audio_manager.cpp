#include "audio_manager.h"
#include "audio_clip.h"

#include "../../xenity.h"

#ifdef __PSP__
#include <pspaudiolib.h>
#include <pspaudio.h>
#include <pspkernel.h>
#include <psppower.h>
#endif

#ifdef __vita__
#include <psp2/audioout.h>
#include <psp2/kernel/threadmgr.h>
#include <malloc.h>
#endif

bool AudioManager::isAdding = false;
int AudioManager::channelCount = 0;
std::vector<Channel *> AudioManager::channels;
int buffSize = 1024 * 16;
MyMutex *AudioManager::myMutex = nullptr;

#define audiosize 2048

short MixSoundToBuffer(short bufferValue, short soundValue)
{
    int newVal = bufferValue + soundValue;
    if (newVal > 32767)
        newVal = 32767;
    else if (newVal < -32768)
        newVal = -32768;

    return newVal;
}

// void FillChannelBuffer(short *buffer, int i, PlayedSound *sound)
void FillChannelBuffer(short *buffer, int length, Channel *channel)
{
    for (int i = 0; i < length; i++)
    {
        buffer[i * 2] = 0;
        buffer[1 + i * 2] = 0;

        AudioManager::myMutex->audioMutex.lock();
        int playedSoundsCount = channel->playedSounds.size();
        for (int soundIndex = 0; soundIndex < playedSoundsCount; soundIndex++)
        {
            PlayedSound *sound = channel->playedSounds[soundIndex];
            if (sound->safeAudioSource.GetIsPlaying())
            {
                float leftPan = std::max(0.0f, std::min(0.5f, 1 - sound->safeAudioSource.GetPanning())) * 2;
                float rightPan = std::max(0.0f, std::min(0.5f, sound->safeAudioSource.GetPanning())) * 2;
                buffer[i * 2] = MixSoundToBuffer(buffer[i * 2], sound->buffer[sound->seekPosition] * sound->safeAudioSource.GetVolume() * leftPan);
                buffer[1 + i * 2] = MixSoundToBuffer(buffer[1 + i * 2], sound->buffer[sound->seekPosition + 1] * sound->safeAudioSource.GetVolume() * rightPan);

                sound->seekNext += ((float)sound->safeAudioSource.audioClip->GetFrenquency());

                while (sound->seekNext >= SOUND_FREQUENCY)
                {
                    sound->seekNext -= SOUND_FREQUENCY;
                    sound->seekPosition += 2;
                    if (sound->seekPosition == buffSize / 2)
                    {
                        sound->needNewRead = true;
                    }
                    else if (sound->seekPosition == buffSize)
                    {
                        sound->seekPosition = 0;
                        sound->needNewRead2 = true;
                    }
                }
            }
        }

        AudioManager::myMutex->audioMutex.unlock();
    }
}

#if defined(__PSP__)

void audioCallback(void *buf, unsigned int length, void *userdata)
{
    Channel *channel = AudioManager::channels[(int)userdata];

    FillChannelBuffer((short *)buf, length, channel);
}
#endif

#if defined(__vita__)
int audio_thread(SceSize args, void *argp)
{
    while (true)
    {
        for (int i = 0; i < AudioManager::channelCount; i++)
        {
            Channel *channel = AudioManager::channels[i];
            if (sceAudioOutGetRestSample(channel->port) == 0)
            {
                int16_t wave_buf[audiosize * 2] = {0};
                FillChannelBuffer((short *)wave_buf, audiosize, channel);
                sceAudioOutOutput(channel->port, wave_buf);
            }
        }
    }
}
#endif

int fillAudioBufferThread(SceSize args, void *argp)
{
    while (true)
    {
        for (int i = 0; i < AudioManager::channelCount; i++)
        {
            Channel *channel = AudioManager::channels[i];

            AudioManager::myMutex->audioMutex.lock();
            int playedSoundsCount = channel->playedSounds.size();
            for (int soundIndex = 0; soundIndex < playedSoundsCount; soundIndex++)
            {
                PlayedSound *sound = channel->playedSounds[soundIndex];
                if (sound->needNewRead)
                {
                    sound->audioSource.lock()->audioClip->FillBuffer(buffSize / 4, 0, sound->buffer);
                    sound->needNewRead = false;
                }
                else if (sound->needNewRead2)
                {
                    sound->audioSource.lock()->audioClip->FillBuffer(buffSize / 4, buffSize / 2, sound->buffer);
                    sound->needNewRead2 = false;
                }
            }
            AudioManager::myMutex->audioMutex.unlock();
        }

#if defined(__PSP__)
        sceKernelDelayThread(10);
#endif
    }
}

Channel::Channel()
{
#if defined(__vita__)
    // This will allow to open only one channel because of SCE_AUDIO_OUT_PORT_TYPE_BGM
    port = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_BGM, audiosize, SOUND_FREQUENCY, (SceAudioOutMode)mode);
    int volA[2] = {vol, vol};
    sceAudioOutSetVolume(port, (SceAudioOutChannelFlag)(SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH), volA);
#endif
}

void AudioManager::Init()
{
    // AudioClip *music0 = new AudioClip("Special_Needs_22050.mp3");
    // AudioClip *music0 = new AudioClip("Special_Needs_32000.mp3");
    // AudioClip *music0 = new AudioClip("Special_Needs_44100.mp3");
    // AudioClip *music0 = new AudioClip("Special_Needs_48000.mp3");
    // AudioClip *music0 = new AudioClip("Special_Needs_88200.wav");
    // AudioClip *music0 = new AudioClip("Wind.mp3");
    // AudioClip *music0 = new AudioClip("Wind.wav");
    // AudioClip *music1 = new AudioClip("Ambient Vol 3.mp3");
    myMutex = new MyMutex();

    Channel *newChannel = new Channel();
    channels.push_back(newChannel);
    channelCount++;

#if defined(__PSP__)
    pspAudioInit();
    for (int i = 0; i < channelCount; i++)
    {
        pspAudioSetChannelCallback(i, audioCallback, (void *)i);
    }

    SceUID thd_id = sceKernelCreateThread("fillAudioBufferThread", fillAudioBufferThread, 0x18, 0x10000, 0, NULL);
    if (thd_id >= 0)
    {
        sceKernelStartThread(thd_id, 0, 0);
    }
#elif defined(__vita__)
    SceUID thd_id = sceKernelCreateThread("audio_thread", audio_thread, 0x40, 0x400000, 0, 0, NULL);
    SceUID thd_id2 = sceKernelCreateThread("fillAudioBufferThread", fillAudioBufferThread, 0x40, 0x400000, 0, 0, NULL);
    if (thd_id >= 0 && thd_id2 >= 0)
    {
        sceKernelStartThread(thd_id2, 0, 0);
        sceKernelStartThread(thd_id, 0, 0);
    }
#endif
}

/// <summary>
/// Add an audio source in the audio source list
/// </summary>
/// <param name="light"></param>
void AudioManager::AddAudioSource(std::weak_ptr<AudioSource> audioSource)
{
    PlayedSound *newPlayedSound = new PlayedSound();
    newPlayedSound->buffer = (short *)calloc(buffSize / 2 * 2, sizeof(short));
    newPlayedSound->audioSource = audioSource;
    newPlayedSound->safeAudioSource = AudioSource(*audioSource.lock());
    newPlayedSound->seekPosition = 0;
    newPlayedSound->needNewRead = true;
    newPlayedSound->needNewRead2 = true;
    AudioManager::myMutex->audioMutex.lock();
    channels[0]->playedSounds.push_back(newPlayedSound);
    AudioManager::myMutex->audioMutex.unlock();
}

void AudioManager::UpdateAudioSource(std::weak_ptr<AudioSource> audioSource)
{
    AudioManager::myMutex->audioMutex.lock();
    Channel *channel = channels[0];
    if (auto as = audioSource.lock())
    {
        int count = channel->playedSounds.size();
        for (int i = 0; i < count; i++)
        {
            PlayedSound *playedSound = channel->playedSounds[i];
            if (playedSound->audioSource.lock() == as)
            {
                playedSound->safeAudioSource = AudioSource(*as);
                break;
            }
        }
    }
    AudioManager::myMutex->audioMutex.unlock();
}

/// <summary>
/// Remove an audio source from the audio source list
/// </summary>
/// <param name="light"></param>
void AudioManager::RemoveAudioSource(std::weak_ptr<AudioSource> audioSource)
{
    AudioManager::myMutex->audioMutex.lock();
    int audioSourceIndex = 0;
    bool found = false;
    Channel *channel = channels[0];

    if (auto as = audioSource.lock())
    {
        int count = channel->playedSounds.size();
        for (int i = 0; i < count; i++)
        {
            if (channel->playedSounds[i]->audioSource.lock() == as)
            {
                audioSourceIndex = i;
                found = true;
                break;
            }
        }
    }

    if (found)
    {
        channel->playedSounds.erase(channel->playedSounds.begin() + audioSourceIndex);
    }
    AudioManager::myMutex->audioMutex.unlock();
}