#include "audio_source.h"
#include "audio_manager.h"

AudioSource::AudioSource()
{
    componentName = "Audio Source";
}

AudioSource::~AudioSource()
{
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