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

#include <thread>
#include <chrono>

bool AudioManager::isAdding = false;
int AudioManager::channelCount = 0;
std::vector<Channel *> AudioManager::channels;
int buffSize = 1024 * 16;
int halfBuffSize = 0;
int quarterBuffSize = 0;
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

void FillChannelBuffer(short *buffer, int length, Channel *channel)
{
    int playedSoundsCount = channel->playedSounds.size();
    bool bufferCleared = false;
    for (int soundIndex = 0; soundIndex < playedSoundsCount; soundIndex++)
    {
        PlayedSound *sound = channel->playedSounds[soundIndex];
        float leftPan = std::max(0.0f, std::min(0.5f, 1 - sound->safeAudioSource.GetPanning())) * 2;
        float rightPan = std::max(0.0f, std::min(0.5f, sound->safeAudioSource.GetPanning())) * 2;
        float leftVolume = sound->safeAudioSource.GetVolume() * leftPan;
        float rightVolume = sound->safeAudioSource.GetVolume() * rightPan;

        if (sound->safeAudioSource.GetIsPlaying())
        {
            for (int i = 0; i < length; i++)
            {
                int leftBufferIndex = i * 2;
                int rightBufferIndex = 1 + i * 2;

                if (!bufferCleared)
                {
                    buffer[leftBufferIndex] = 0;
                    buffer[rightBufferIndex] = 0;
                }

                buffer[leftBufferIndex] = MixSoundToBuffer(buffer[leftBufferIndex], sound->buffer[sound->seekPosition] * leftVolume);
                buffer[rightBufferIndex] = MixSoundToBuffer(buffer[rightBufferIndex], sound->buffer[sound->seekPosition + 1] * rightVolume);

                sound->seekNext += sound->safeAudioSource.audioClip->GetFrenquency();

                while (sound->seekNext >= SOUND_FREQUENCY)
                {
                    sound->seekNext -= SOUND_FREQUENCY;
                    sound->seekPosition += 2;

                    if (sound->safeAudioSource.audioClip != nullptr && (sound->safeAudioSource.audioClip->GetSeekPosition() >= sound->safeAudioSource.audioClip->GetSampleCount()))
                    {
                        sound->safeAudioSource.audioClip->ResetSeek();
                        sound->seekPosition = 0;
                    }
                    else if (sound->seekPosition == halfBuffSize)
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
            bufferCleared = true;
        }
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
        AudioManager::myMutex->audioMutex.lock();
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
        AudioManager::myMutex->audioMutex.unlock();
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
#endif

int fillAudioBufferThread(SceSize args, void *argp)
{
    while (true)
    {
        AudioManager::myMutex->audioMutex.lock();
        for (int i = 0; i < AudioManager::channelCount; i++)
        {
            Channel *channel = AudioManager::channels[i];

            int playedSoundsCount = channel->playedSounds.size();
            for (int soundIndex = 0; soundIndex < playedSoundsCount; soundIndex++)
            {
                PlayedSound *sound = channel->playedSounds[soundIndex];
                if (sound->safeAudioSource.audioClip != nullptr)
                {
                    if (sound->needNewRead)
                    {
                        sound->safeAudioSource.audioClip->FillBuffer(quarterBuffSize, 0, sound->buffer);
                        sound->needNewRead = false;
                    }
                    else if (sound->needNewRead2)
                    {

                        sound->safeAudioSource.audioClip->FillBuffer(quarterBuffSize, halfBuffSize, sound->buffer);
                        sound->needNewRead2 = false;
                    }
                }
            }
        }
        AudioManager::myMutex->audioMutex.unlock();
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
    halfBuffSize = buffSize / 2;
    quarterBuffSize = buffSize / 4;

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

PlayedSound::~PlayedSound()
{
    if (buffer)
        free(buffer);
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
    channels[0]->playedSounds.push_back(newPlayedSound);
}

void AudioManager::UpdateAudioSource(std::weak_ptr<AudioSource> audioSource)
{
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
}

/// <summary>
/// Remove an audio source from the audio source list
/// </summary>
/// <param name="light"></param>
void AudioManager::RemoveAudioSource(std::weak_ptr<AudioSource> audioSource)
{
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
        delete channel->playedSounds[audioSourceIndex];
        channel->playedSounds.erase(channel->playedSounds.begin() + audioSourceIndex);
    }
}