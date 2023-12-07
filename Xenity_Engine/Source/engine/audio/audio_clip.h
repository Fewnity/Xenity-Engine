#pragma once
#include <engine/api.h>

#include <memory>
#include <engine/file_system/file_reference.h>
#include <engine/reflection/reflection.h>

class API AudioClip : public FileReference, public Reflective
{
public:
	AudioClip();
	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData() override;
	static std::shared_ptr<AudioClip> MakeAudioClip();
private:
};