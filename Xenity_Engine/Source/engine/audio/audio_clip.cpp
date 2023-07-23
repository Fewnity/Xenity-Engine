#include "audio_clip.h"

AudioClip::AudioClip(std::string filePath)
{
    this->filePath = filePath;
}

std::unordered_map<std::string, Variable> AudioClip::GetReflection()
{
	std::unordered_map<std::string, Variable> reflectedVariables;
	reflectedVariables.insert_or_assign("fileId", fileId);
	return reflectedVariables;
}