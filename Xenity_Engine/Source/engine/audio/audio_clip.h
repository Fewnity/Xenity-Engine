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

class API AudioClip : public FileReference, public Reflective
{
public:
	AudioClip();
	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflection() override;
	static std::shared_ptr<AudioClip> MakeAudioClip();
private:
};