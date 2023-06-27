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

class AudioClipStream
{
public:
    void OpenStream(std::string fileName);
    ~AudioClipStream();
    void FillBuffer(int size, int bufferOffset, short *buff);
    short *GetBuffer(int size);
    int GetFrenquency();
    int64_t GetSampleCount();
    void ResetSeek();
    int64_t GetSeekPosition();

private:
    AudioType type = Null;
    drmp3 mp3;
    drwav wav;
    int64_t sampleCount = 0;
};