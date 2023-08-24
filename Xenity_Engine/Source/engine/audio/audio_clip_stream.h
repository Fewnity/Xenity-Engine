#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include "dr_mp3.h"
#include "dr_wav.h"

#include <string>

class Channel;

enum AudioType
{
    Null,
    Wav,
    Mp3,
};

class API AudioClipStream
{
public:
    void OpenStream(std::string fileName);
    ~AudioClipStream();

    /**
    * Fill audio buffer
    */
    void FillBuffer(int size, int bufferOffset, short *buff);

    /**
    * Get audio clip frequency
    */
    int GetFrequency();

    /**
    * Get audio clip sample count
    */
    int64_t GetSampleCount();

    /**
    * Reset seek position
    */
    void ResetSeek();

    /**
    * Get seek position
    */
    int64_t GetSeekPosition();

private:
    AudioType type = Null;
    drmp3 mp3;
    drwav wav;
    int64_t sampleCount = 0;
};