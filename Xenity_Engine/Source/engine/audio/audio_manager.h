#pragma once

class AudioManager
{
public:
    static void Init();
    static void Update();

private:
    int channelCount = 4;
};