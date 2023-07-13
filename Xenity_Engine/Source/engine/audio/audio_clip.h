#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <string>

class API AudioClip
{
public:
    AudioClip() = delete;
    AudioClip(std::string filePath);
    std::string filePath = "";

private:
};