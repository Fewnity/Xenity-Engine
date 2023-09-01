#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <string>
#include <memory>
#include "../file_system/file_reference.h"
#include "../reflection/reflection.h"

class API AudioClip : public FileReference, public Reflection
{
public:
    AudioClip() = delete;
    AudioClip(std::string filePath);
    std::unordered_map<std::string, Variable> GetReflection();
    std::unordered_map<std::string, Variable> GetMetaReflection();
    std::string filePath = "";
    static std::shared_ptr<AudioClip> MakeAudioClip(std::string filePath);
private:
};