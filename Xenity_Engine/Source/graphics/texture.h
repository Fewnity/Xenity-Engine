#pragma once

#include <string>

class Texture
{
public:
	Texture() = delete;
	Texture(std::string filePath, unsigned int textureIndex);
	unsigned int GetTextureIndex();
	unsigned int GetTextureId();

private:
	void LoadTexture(std::string filePath);
	unsigned int textureId, textureIndex;
	int width, height, nrChannels;
};

