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

class AudioClip
{
public:
    AudioClip() = delete;
    AudioClip(std::string filePath);
    ~AudioClip();
    void FillBuffer(int size, int bufferOffset, short *buff);
    short *GetBuffer(int size);
    int GetFrenquency();

private:
    AudioType type = Null;
    drmp3 mp3;
    drwav wav;
};