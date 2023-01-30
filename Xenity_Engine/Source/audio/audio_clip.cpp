#include "audio_clip.h"
#include "../file_system/file.h"

AudioClip::AudioClip(std::string filePath)
{
	this->filePath = File::GetGamePath() + filePath;
}

AudioClip::~AudioClip()
{
}

std::string AudioClip::GetFilePath()
{
	return this->filePath;
}