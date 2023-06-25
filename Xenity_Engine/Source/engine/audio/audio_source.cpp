#include "audio_source.h"
#include "audio_manager.h"

AudioSource::AudioSource()
{
    componentName = "Audio Source";
}

AudioSource::~AudioSource()
{
}

void AudioSource::SetVolume(float volume)
{
    if (volume > 1)
        volume = 1;
    else if (volume < 0)
        volume = 0;
    this->volume = volume;
    AudioManager::UpdateAudioSource(std::dynamic_pointer_cast<AudioSource>(shared_from_this()));
}

void AudioSource::SetPanning(float panning)
{
    if (panning > 1)
        panning = 1;
    else if (panning < 0)
        panning = 0;
    this->pan = panning;
    AudioManager::UpdateAudioSource(std::dynamic_pointer_cast<AudioSource>(shared_from_this()));
}

void AudioSource::SetLoop(bool isLooping)
{
    this->loop = isLooping;
    AudioManager::UpdateAudioSource(std::dynamic_pointer_cast<AudioSource>(shared_from_this()));
}

void AudioSource::Play()
{
    isPlaying = true;
    AudioManager::UpdateAudioSource(std::dynamic_pointer_cast<AudioSource>(shared_from_this()));
}

void AudioSource::Resume()
{
    isPlaying = true;
    AudioManager::UpdateAudioSource(std::dynamic_pointer_cast<AudioSource>(shared_from_this()));
}

void AudioSource::Pause()
{
    isPlaying = false;
    AudioManager::UpdateAudioSource(std::dynamic_pointer_cast<AudioSource>(shared_from_this()));
}

void AudioSource::Stop()
{
    isPlaying = false;
    AudioManager::UpdateAudioSource(std::dynamic_pointer_cast<AudioSource>(shared_from_this()));
}