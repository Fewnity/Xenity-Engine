#include "audio_manager.h"
#include "audio_clip.h"
#include "audio_clip_stream.h"

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
    for (int i = 0; i < length; i++)
    {
        int leftBufferIndex = i * 2;
        int rightBufferIndex = 1 + i * 2;
        buffer[leftBufferIndex] = 0;
        buffer[rightBufferIndex] = 0;
    }

    AudioManager::myMutex->Lock();

    int playedSoundsCount = channel->playedSounds.size();
    for (int soundIndex = 0; soundIndex < playedSoundsCount; soundIndex++)
    {
        auto sound = channel->playedSounds[soundIndex];

        float leftPan = std::max(0.0f, std::min(0.5f, 1 - sound->pan)) * 2;
        float rightPan = std::max(0.0f, std::min(0.5f, sound->pan)) * 2;
        float leftVolume = sound->volume * leftPan;
        float rightVolume = sound->volume * rightPan;
        bool isPlaying = sound->isPlaying;

        if (isPlaying)
        {
            for (int i = 0; i < length; i++)
            {
                int leftBufferIndex = i * 2;
                int rightBufferIndex = 1 + i * 2;

                buffer[leftBufferIndex] = MixSoundToBuffer(buffer[leftBufferIndex], sound->buffer[sound->seekPosition] * leftVolume);
                buffer[rightBufferIndex] = MixSoundToBuffer(buffer[rightBufferIndex], sound->buffer[sound->seekPosition + 1] * rightVolume);

                sound->seekNext += sound->audioClipStream->GetFrenquency();

                while (sound->seekNext >= SOUND_FREQUENCY)
                {
                    sound->seekNext -= SOUND_FREQUENCY;
                    sound->seekPosition += 2;

                    if (sound->audioClipStream->GetSeekPosition() >= sound->audioClipStream->GetSampleCount())
                    {
                        sound->audioClipStream->ResetSeek();
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
        }
    }
    AudioManager::myMutex->Unlock();
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

            AudioManager::myMutex->Lock();
            int playedSoundsCount = channel->playedSounds.size();
            for (int soundIndex = 0; soundIndex < playedSoundsCount; soundIndex++)
            {
                auto sound = channel->playedSounds[soundIndex];
                if (sound->needNewRead)
                {
                    sound->audioClipStream->FillBuffer(quarterBuffSize, 0, sound->buffer);
                    sound->needNewRead = false;
                }
                else if (sound->needNewRead2)
                {
                    sound->audioClipStream->FillBuffer(quarterBuffSize, halfBuffSize, sound->buffer);
                    sound->needNewRead2 = false;
                }
            }
            AudioManager::myMutex->Unlock();
        }

        if (Engine::valueFree)
        {
            AudioManager::myMutex->Lock();
            Channel *channel = AudioManager::channels[0];
            int count = channel->playedSounds.size();
            for (int i = 0; i < count; i++)
            {
                auto playedSound = channel->playedSounds[i];
                playedSound->volume = playedSound->audioSource->GetVolume();
                playedSound->pan = playedSound->audioSource->GetPanning();
                playedSound->isPlaying = playedSound->audioSource->GetIsPlaying();
            }
            AudioManager::myMutex->Unlock();
        }

        sceKernelDelayThread(1);
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

    myMutex = new MyMutex();
#if defined(__vita__)
    myMutex->mutexid = sceKernelCreateMutex("MyMutex", 0, 1, NULL);
#endif
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

void AudioManager::Update()
{
}

PlayedSound::~PlayedSound()
{
    delete audioClipStream;
    if (buffer)
        free(buffer);
}

void AudioManager::PlayAudioSource(std::weak_ptr<AudioSource> audioSource)
{
    AudioManager::myMutex->Lock();
    Channel *channel = channels[0];
    bool found = false;
    if (auto as = audioSource.lock())
    {
        if (as->audioClip == nullptr)
            return;

        int count = channel->playedSounds.size();
        for (int i = 0; i < count; i++)
        {
            auto playedSound = channel->playedSounds[i];
            if (playedSound->audioSource == as)
            {
                found = true;
                break;
            }
        }
    }

    if (!found)
    {
        std::shared_ptr<PlayedSound> newPlayedSound = std::make_shared<PlayedSound>();
        newPlayedSound->buffer = (short *)calloc(buffSize / 2 * 2, sizeof(short));
        AudioClipStream *newAudioClipStream = new AudioClipStream();
        newPlayedSound->audioClipStream = newAudioClipStream;
        newAudioClipStream->OpenStream(audioSource.lock()->audioClip->filePath);
        newPlayedSound->audioSource = audioSource.lock();
        newPlayedSound->seekPosition = 0;
        newPlayedSound->needNewRead = true;
        newPlayedSound->needNewRead2 = true;

        newPlayedSound->volume = newPlayedSound->audioSource->GetVolume();
        newPlayedSound->pan = newPlayedSound->audioSource->GetPanning();
        newPlayedSound->isPlaying = newPlayedSound->audioSource->GetIsPlaying();
        newPlayedSound->loop = true;
        channels[0]->playedSounds.push_back(newPlayedSound);
        AudioManager::myMutex->Unlock();
    }
}

void AudioManager::StopAudioSource(std::weak_ptr<AudioSource> audioSource)
{
    AudioManager::myMutex->Lock();
    int audioSourceIndex = 0;
    bool found = false;
    Channel *channel = channels[0];

    if (auto as = audioSource.lock())
    {
        int count = channel->playedSounds.size();
        for (int i = 0; i < count; i++)
        {
            if (channel->playedSounds[i]->audioSource == as)
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
    AudioManager::myMutex->Unlock();
}

/// <summary>
/// Remove an audio source from the audio source list
/// </summary>
/// <param name="light"></param>
void AudioManager::RemoveAudioSource(std::weak_ptr<AudioSource> audioSource)
{
    AudioManager::myMutex->Lock();
    int audioSourceIndex = 0;
    bool found = false;
    Channel *channel = channels[0];

    if (auto as = audioSource.lock())
    {
        int count = channel->playedSounds.size();
        for (int i = 0; i < count; i++)
        {
            if (channel->playedSounds[i]->audioSource == as)
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
    AudioManager::myMutex->Unlock();
}