#include "texture.h"
#include <glad/glad.h>
#include "../asset_manager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "../file_system/file.h"

Texture::Texture(std::string filePath) {
	textureIndex = AssetManager::GetTextureCount();
	LoadTexture(filePath);
	AssetManager::AddTexture(this);
}

Texture::~Texture()
{
	AssetManager::RemoveTexture(this);
	glDeleteTextures(1, &textureId);
}

void Texture::LoadTexture(std::string filePath) {
	unsigned char* data = File::LoadTextureData(filePath, this->width, this->height, this->nrChannels);

	glActiveTexture(GL_TEXTURE0 + textureIndex);
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	if (data) {
		if (this->nrChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//printf("Texture loaded -> size : %dx%d; nrChannels: %d", width, height, nrChannels);
	}
	else {
		printf("Texture can't be loaded. Path : \"%s\"\n", filePath.c_str());
	}
	stbi_image_free(data);
}

unsigned int Texture::GetTextureId()
{
	return  textureId;
}

unsigned int Texture::GetTextureIndex()
{
	return  textureIndex;
}