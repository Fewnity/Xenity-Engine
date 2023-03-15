#include "texture.h"
#include <glad/glad.h>
#include "../asset_manager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "../file_system/file.h"
#include "../engine_settings.h"
#include <iostream>
#include "../debug/debug.h"

#pragma region Constructors / Destructor

Texture::Texture(const std::string filePath)
{
	CreateTextutre(filePath, Bilinear, true);
}

Texture::Texture(const std::string filePath, const Filter filter, const bool useMipMap)
{
	CreateTextutre(filePath, filter, useMipMap);
}

Texture::~Texture()
{
	AssetManager::RemoveTexture(this);
	glDeleteTextures(1, &textureId);
}

#pragma endregion

#pragma region Data loading

/// <summary>
/// Create the texture from the file path and texture settings
/// </summary>
/// <param name="filePath">File path</param>
/// <param name="filter">Filter to use</param>
/// <param name="useMipMap">Will texture use mipmap</param>
void Texture::CreateTextutre(const std::string filePath, const Filter filter, const bool useMipMap)
{
	this->filter = filter;
	this->useMipMap = useMipMap;

	LoadTexture(filePath);

	AssetManager::AddTexture(this);
}

/// <summary>
/// Load texture from a filepath
/// </summary>
/// <param name="filePath"></param>
void Texture::LoadTexture(const std::string filePath) {
	Debug::Print("Loading texture...");

	stbi_set_flip_vertically_on_load(false);
	unsigned char* data = File::LoadTextureData(filePath, this->width, this->height, this->nrChannels);

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	if (data) {
		if (this->nrChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		if (useMipMap)
			glGenerateMipmap(GL_TEXTURE_2D);

		UpdateTextureFilter();

		//printf("Texture loaded -> size : %dx%d; nrChannels: %d", width, height, nrChannels);
	}
	else
	{
		Debug::Print("Texture can't be loaded. Path: " + filePath);
	}
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);
}

#pragma endregion

#pragma region Accessors

/// <summary>
/// Set texture's filter
/// </summary>
/// <param name="filter"></param>
void Texture::SetFilter(const Filter filter)
{
	this->filter = filter;
	UpdateTextureFilter();
}

int Texture::GetWidth() const
{
	return width;
}

int Texture::GetHeight() const
{
	return height;
}

/// <summary>
/// Get texture id
/// </summary>
/// <returns></returns>
unsigned int Texture::GetTextureId() const
{
	return textureId;
}

#pragma endregion

/// <summary>
/// Update texture if the filter has changed
/// </summary>
void Texture::UpdateTextureFilter()
{
	glBindTexture(GL_TEXTURE_2D, textureId);
	int minFilterValue = 0;
	int magfilterValue = 0;
	if (filter == Bilinear)
	{
		if (useMipMap)
		{
			minFilterValue = GL_LINEAR_MIPMAP_LINEAR;
			magfilterValue = GL_LINEAR;
		}
		else
		{
			minFilterValue = GL_LINEAR;
			magfilterValue = GL_LINEAR;
		}
	}
	else if (filter == Point)
	{
		if (useMipMap)
		{
			minFilterValue = GL_NEAREST_MIPMAP_NEAREST;
			magfilterValue = GL_NEAREST;
		}
		else
		{
			minFilterValue = GL_NEAREST;
			magfilterValue = GL_NEAREST;
		}
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilterValue);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magfilterValue);

	float anisotropicValue = 16;
	switch (EngineSettings::anisotropicLevel)
	{
	case Texture::X0:
		anisotropicValue = 1;
		break;
	case Texture::X2:
		anisotropicValue = 2;
		break;
	case Texture::X4:
		anisotropicValue = 4;
		break;
	case Texture::X8:
		anisotropicValue = 8;
		break;
	case Texture::X16:
		anisotropicValue = 16;
		break;
	}

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, anisotropicValue);
}