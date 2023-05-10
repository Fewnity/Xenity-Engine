#include "texture.h"

#include "../../engine/asset_managent/asset_manager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "../file_system/file.h"
#include "../engine_settings.h"
#include <iostream>
#include "../debug/debug.h"

#include "../engine.h"
#include "../graphics/renderer/renderer.h"

#pragma region Constructors / Destructor

Texture::Texture(const std::string filePath, std::string name)
{
	CreateTexture(filePath, name, Bilinear, true);
}

Texture::Texture(const std::string filePath, std::string name, const Filter filter, const bool useMipMap)
{
	CreateTexture(filePath, name, filter, useMipMap);
}

Texture::Texture(const int textureId, const int channelCount, const int width, const int height)
{
	this->textureId = textureId;
	this->nrChannels = channelCount;
	this->width = width;
	this->height = height;
	useMipMap = false;
}

Texture::~Texture()
{
	AssetManager::RemoveTexture(this);
	Engine::renderer->DeleteTexture(this);
}

#pragma endregion

#pragma region Data loading

/// <summary>
/// Create the texture from the file path and texture settings
/// </summary>
/// <param name="filePath">File path</param>
/// <param name="filter">Filter to use</param>
/// <param name="useMipMap">Will texture use mipmap</param>
void Texture::CreateTexture(const std::string filePath, std::string name, const Filter filter, const bool useMipMap)
{
	this->filter = filter;
	this->useMipMap = useMipMap;
	this->name = name;

	LoadTexture(filePath);

	AssetManager::AddTexture(this);
}

/// <summary>
/// Load texture from a filepath
/// </summary>
/// <param name="filePath"></param>
void Texture::LoadTexture(const std::string filePath) 
{
	Debug::Print("Loading texture...");

	stbi_set_flip_vertically_on_load(false);
	unsigned char* data = File::LoadTextureData(filePath, this->width, this->height, this->nrChannels);

	textureId = Engine::renderer->GenerateTextureId();
	Engine::renderer->BindTexture(this);

	if (data) {
		Engine::renderer->SetTextureData(this, data);

		if (useMipMap)
			Engine::renderer->GenerateMipmap();

		UpdateTextureFilter();

		//printf("Texture loaded -> size : %dx%d; nrChannels: %d", width, height, nrChannels);
	}
	else
	{
		Debug::Print("Texture can't be loaded. Path: " + filePath);
	}
	stbi_image_free(data);

	Engine::renderer->BindTexture(nullptr);
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

void Texture::SetWrapMode(const WrapMode mode)
{
	wrapMode = mode;
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

void Texture::SetPixelPerUnit(int value)
{
	pixelPerUnit = value;
}

int Texture::GetPixelPerUnit() const
{
	return pixelPerUnit;
}

int Texture::GetChannelCount() const
{
	return nrChannels;
}

bool Texture::GetUseMipmap() const
{
	return useMipMap;
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
	Engine::renderer->BindTexture(this);
	Engine::renderer->SetTextureFilter(this, filter);
	Engine::renderer->SetTextureWrapMode(wrapMode);
	Engine::renderer->SetAnisotropyLevel(EngineSettings::anisotropicLevel);
}