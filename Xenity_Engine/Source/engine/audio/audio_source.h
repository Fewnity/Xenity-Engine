#pragma once

#include "../component.h"

class AudioClip;

class AudioSource : public Component
{
public:
    AudioSource();
    ~AudioSource();
    void Play();
    void Resume();
    void Pause();
    void Stop();

    AudioClip *audioClip = nullptr;

    void SetVolume(float volume);

    void SetPanning(float panning);

    void SetLoop(bool isLooping);

    float GetVolume()
    {
        return volume;
    }

    float GetPanning()
    {
        return pan;
    }

    bool GetIsPlaying()
    {
        return isPlaying;
    }

private:
    float volume = 1;
    float pan = 0.5;
    bool loop = true;
    bool isPlaying = false;
};