#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include "../component.h"
#include <memory>

class AudioClip;

class API AudioSource : public Component
{
public:
    AudioSource();
    ~AudioSource();
    //void SetReflection();

    std::unordered_map<std::string, ReflectionEntry> GetReflection();

    void Awake();

    /**
    * Play audio
    */
    void Play();

    /**
    * Resume audio
    */
    void Resume();

    /**
    * Pause audio
    */
    void Pause();

    /**
    * Stop audio
    */
    void Stop();

    std::shared_ptr<AudioClip> audioClip = nullptr;

    /**
    * Set volume
    */
    void SetVolume(float volume);

    /**
    * Set panning
    */
    void SetPanning(float panning);

    /**
    * Set is looping
    */
    void SetLoop(bool isLooping);

    /**
    * Get volume
    */
    float GetVolume()
    {
        return volume;
    }

    /**
    * Get panning
    */
    float GetPanning()
    {
        return pan;
    }

    /**
    * Get is playing
    */
    bool GetIsPlaying()
    {
        return isPlaying;
    }

    /**
    * Get is looping
    */
    bool GetIsLooping() 
    {
        return loop;
    }

    std::shared_ptr<AudioSource> GetThisShared()
    {
        return std::dynamic_pointer_cast<AudioSource>(shared_from_this());
    }

private:
    float volume = 1;
    float pan = 0.5;
    bool loop = true;
    bool isPlaying = false;
    bool playOnAwake = true;
};