#include "audio_clip.h"
#include "../file.h"

AudioClip::AudioClip(std::string filePath)
{
	this->filePath = File::GetGamePath() + filePath;
}

std::string AudioClip::GetFilePath()
{
	return this->filePath;
}