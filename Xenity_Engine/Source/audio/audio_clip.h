#pragma once

#include <string>

class AudioClip
{
public:
	AudioClip() = delete;
	AudioClip(std::string filePath);
	~AudioClip();
	std::string GetFilePath();
private:
	std::string filePath = "";
};

