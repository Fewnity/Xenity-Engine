#include "audio_source.h"
#include "audio_manager.h"
#include "../engine.h"
#include "../../xenity.h"

AudioSource::AudioSource()
{
    componentName = "AudioSource";
    //SetReflection();
}

/*void AudioSource::SetReflection()
{
    componentName = "AudioSource";
    reflectedVariables["volume"] = &volume;
    reflectedVariables["pan"] = &pan;
    reflectedVariables["isPlaying"] = &isPlaying;
    reflectedVariables["loop"] = &loop;
}*/

std::unordered_map<std::string, Variable> AudioSource::GetReflection()
{
    std::unordered_map<std::string, Variable> reflectedVariables;
    reflectedVariables.insert_or_assign("volume", volume);
    reflectedVariables.insert_or_assign("pan", pan);
    //reflectedVariables.insert_or_assign("isPlaying", isPlaying);
    reflectedVariables.insert_or_assign("playOnAwake", playOnAwake);
    reflectedVariables.insert_or_assign("loop", loop);
    reflectedVariables.insert_or_assign("audioClip", audioClip);
    return reflectedVariables;
}

AudioSource::~AudioSource()
{
}

void AudioSource::Awake() 
{
    if(playOnAwake)
        Play();
}

void AudioSource::SetVolume(float volume)
{
    if (volume > 1)
        volume = 1;
    else if (volume < 0)
        volume = 0;
    this->volume = volume;
}

void AudioSource::SetPanning(float panning)
{
    if (panning > 1)
        panning = 1;
    else if (panning < 0)
        panning = 0;
    this->pan = panning;
}

void AudioSource::SetLoop(bool isLooping)
{
    this->loop = isLooping;
}

void AudioSource::Play()
{
    if (audioClip != nullptr) 
    {
        isPlaying = true;
        AudioManager::PlayAudioSource(std::dynamic_pointer_cast<AudioSource>(shared_from_this()));
    }
}

void AudioSource::Resume()
{
    if (audioClip != nullptr)
    {
        isPlaying = true;
    }
}

void AudioSource::Pause()
{
    isPlaying = false;
}

void AudioSource::Stop()
{
    isPlaying = false;
    AudioManager::StopAudioSource(std::dynamic_pointer_cast<AudioSource>(shared_from_this()));
}