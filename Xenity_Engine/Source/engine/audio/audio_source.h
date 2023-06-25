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

    void SetVolume(float volume)
    {
        if (volume > 1)
            volume = 1;
        else if (volume < 0)
            volume = 0;
        this->volume = volume;
    }

    void SetPanning(float panning)
    {
        if (panning > 1)
            panning = 1;
        else if (panning < 0)
            panning = 0;
        this->pan = panning;
    }

    void SetLoop(bool isLooping)
    {
        this->loop = isLooping;
    }

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