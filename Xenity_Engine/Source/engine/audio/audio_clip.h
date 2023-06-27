#pragma once

#include <string>

class AudioClip
{
public:
    AudioClip() = delete;
    AudioClip(std::string filePath);
    std::string filePath = "";

private:
};