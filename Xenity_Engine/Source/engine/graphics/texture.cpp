#include "texture.h"
#include "../../xenity.h"

#include "renderer/renderer.h"

#include "../../../include/stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../../../include/stb_image_resize.h"
#include <malloc.h>
#include <string>

#if defined(__vita__)
#include <vitaGL.h>
#elif defined(__PSP__)
#include "../../psp/gu2gl.h"
#include <pspkernel.h>
#include <vram.h>
#endif

Texture::Texture()
{
}

Texture::Texture(const std::string filePath, std::string name, bool loadInVram)
{
	this->inVram = loadInVram;
	CreateTexture(filePath, name, Bilinear, false);
}

Texture::Texture(const std::string filePath, std::string name, const Filter filter, const bool useMipMap, bool loadInVram)
{
	this->inVram = loadInVram;
	CreateTexture(filePath, name, filter, useMipMap);
}

Texture::Texture(const int textureId, const int channelCount, const int width, const int height, bool loadInVram)
{
	this->textureId = textureId;
	this->nrChannels = channelCount;
	this->width = width;
	this->height = height;
	this->inVram = loadInVram;
	useMipMap = false;
}

Texture::Texture(unsigned char* data, const int channelCount, const int width, const int height, bool loadInVram)
{
	// this->textureId = textureId;
	this->nrChannels = channelCount;
	this->width = width;
	this->height = height;
	this->inVram = loadInVram;

	useMipMap = false;
	if (data != nullptr)
	{
		SetData(data);
		isValid = true;
	}
}

std::unordered_map<std::string, Variable> Texture::GetReflection()
{
	std::unordered_map<std::string, Variable> reflectedVariables;
	reflectedVariables.insert_or_assign("fileId", fileId);
	return reflectedVariables;
}

std::unordered_map<std::string, Variable> Texture::GetMetaReflection()
{
	std::unordered_map<std::string, Variable> reflectedVariables;
	reflectedVariables.insert_or_assign("inVram", inVram);
	reflectedVariables.insert_or_assign("useMipMap", useMipMap);
	reflectedVariables.insert_or_assign("mipmaplevelCount", mipmaplevelCount);
	reflectedVariables.insert_or_assign("filter", (int&)filter);
	reflectedVariables.insert_or_assign("wrapMode", (int&)wrapMode);
	return reflectedVariables;
}

Texture::~Texture()
{
	AssetManager::RemoveTexture(this);
	// Engine::renderer->DeleteTexture(this);
}

void Texture::LoadFileReference()
{
	if (!isLoaded)
	{
		isLoaded = true;
		AssetManager::AddFileReference(this);
		CreateTexture(file->GetPath(), file->GetPath(), filter, useMipMap);
	}
}

void Texture::UnloadFileReference()
{
	if (isLoaded)
	{
		isLoaded = false;
		AssetManager::RemoveFileReference(this);
		Unload();
	}
}

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

#if defined(__PSP__)
void swizzle_fast(u8* out, const u8* in, const unsigned int width, const unsigned int height)
{
	unsigned int blockx, blocky;
	unsigned int j;

	unsigned int width_blocks = (width / 16);
	unsigned int height_blocks = (height / 8);

	unsigned int src_pitch = (width - 16) / 4;
	unsigned int src_row = width * 8;

	const u8* ysrc = in;
	u32* dst = (u32*)out;

	for (blocky = 0; blocky < height_blocks; ++blocky)
	{
		const u8* xsrc = ysrc;
		for (blockx = 0; blockx < width_blocks; ++blockx)
		{
			const u32* src = (u32*)xsrc;
			for (j = 0; j < 8; ++j)
			{
				*(dst++) = *(src++);
				*(dst++) = *(src++);
				*(dst++) = *(src++);
				*(dst++) = *(src++);
				src += src_pitch;
			}
			xsrc += 16;
		}
		ysrc += src_row;
	}
}

// See https://github.com/pspdev/pspsdk/blob/master/src/debug/scr_printf.c
void copy_texture_data(void* dest, const void* src, int width, int height, const int destType, const int srcType)
{
	if (destType == srcType)
	{
		if (srcType == GU_PSM_4444 || srcType == GU_PSM_5650 || srcType == GU_PSM_5551)
		{
			height /= 2;
		}
		for (unsigned int y = 0; y < height; y++)
		{
			for (unsigned int x = 0; x < width; x++)
			{
				((unsigned int*)dest)[x + y * width] = ((unsigned int*)src)[x + y * width];
			}
		}
	}
	else
	{
		if (srcType == GU_PSM_8888 && destType == GU_PSM_4444)
		{
			for (unsigned int y = 0; y < height; y++)
			{
				for (unsigned int x = 0; x < width; x++)
				{
					uint32_t srcPixel = ((uint32_t*)src)[x + y * width];

					uint16_t r = (srcPixel >> 24) & 0xFF;
					uint16_t g = (srcPixel >> 16) & 0xFF;
					uint16_t b = (srcPixel >> 8) & 0xFF;
					uint16_t a = srcPixel & 0xFF;

					uint16_t destPixel = (r >> 4) << 12 | (g >> 4) << 8 | (b >> 4) << 4 | (a >> 4);

					((uint16_t*)dest)[x + y * width] = destPixel;
				}
			}
		}
		else if (srcType == GU_PSM_8888 && destType == GU_PSM_5650)
		{
			for (unsigned int y = 0; y < height; y++)
			{
				for (unsigned int x = 0; x < width; x++)
				{
					uint32_t srcPixel = ((uint32_t*)src)[x + y * width];

					uint16_t r = (srcPixel >> 19) & 0x1F;
					uint16_t g = (srcPixel >> 10) & 0x3F;
					uint16_t b = (srcPixel >> 3) & 0x1F;

					uint16_t destPixel = b | (g << 5) | (r << 11);

					((uint16_t*)dest)[x + y * width] = destPixel;
				}
			}
		}
		else if (srcType == GU_PSM_8888 && destType == GU_PSM_5551)
		{
			for (unsigned int y = 0; y < height; y++)
			{
				for (unsigned int x = 0; x < width; x++)
				{
					uint32_t srcPixel = ((uint32_t*)src)[x + y * width];

					uint16_t a = (srcPixel >> 24) ? 0x8000 : 0;
					uint16_t b = (srcPixel >> 19) & 0x1F;
					uint16_t g = (srcPixel >> 11) & 0x1F;
					uint16_t r = (srcPixel >> 3) & 0x1F;

					uint16_t destPixel = a | r | (g << 5) | (b << 10);

					((uint16_t*)dest)[x + y * width] = destPixel;
				}
			}
		}
	}
}

void Texture::SetTextureLevel(int level, const unsigned char* texData)
{
	bool needResize = false;
	int bytePerPixel = getColorByteCount(type);

	int diviser = (int)pow(2, level);

	int resizedPW = pW / diviser;
	int resizedPH = pH / diviser;

	int byteCount = (resizedPW * resizedPH) * bytePerPixel;

	unsigned int* dataBuffer = (unsigned int*)memalign(16, byteCount);

	if (level != 0 || (width != pW || height != pH))
	{
		needResize = true;
	}

	if (needResize)
	{
		unsigned char* resizedData = (unsigned char*)malloc((resizedPW * resizedPH)*4);
		stbir_resize_uint8(texData, width, height, 0, resizedData, resizedPW, resizedPH, 0, 4);
		copy_texture_data(dataBuffer, resizedData, resizedPW, resizedPH, type, GU_PSM_8888);
		free(resizedData);
	}
	else
	{
		copy_texture_data(dataBuffer, texData, pW, pH, type, GU_PSM_8888);
	}

	if (resizedPW > 256 || resizedPH > 256)
		inVram = false;

	// Allocate memory in ram or vram
	if (inVram)
	{
		unsigned int* newData = (unsigned int*)vramalloc(byteCount);
		// If there is no more free vram
		if (!newData)
		{
			Debug::PrintWarning("No more free vram");
			newData = (unsigned int*)memalign(16, byteCount);
			inVram = false;
		}
		data.push_back((unsigned int*)newData);
	}
	else
	{
		data.push_back((unsigned int*)memalign(16, byteCount));
	}

	// Place image data in the memory
	swizzle_fast((u8*)data[level], (const u8*)dataBuffer, resizedPW * bytePerPixel, resizedPH);
	free(dataBuffer);
	sceKernelDcacheWritebackInvalidateAll();
}

#endif

void Texture::SetData(const unsigned char* texData)
{
	// sceGeEdramSetSize(4096);
	// The psp needs a pow2 sized texture
#if defined(__PSP__)
	//type = GU_PSM_8888;
	//type = GU_PSM_4444;
	//type = GU_PSM_5650;
	type = GU_PSM_5551;

	// Get pow2 size
	pW = Math::nextPow2(width);
	pH = Math::nextPow2(height);

	SetTextureLevel(0, texData);
	if (useMipMap)
	{
		SetTextureLevel(1, texData);
		// SetTextureLevel(2, texData);
		// SetTextureLevel(3, texData);
		mipmaplevelCount = 1;
	}

#endif

#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	textureId = Engine::renderer->CreateNewTexture();
	Engine::renderer->BindTexture(this);
	unsigned int alpha = 0x1906;
	Engine::renderer->SetTextureData(this, alpha, texData);
#endif
}

void Texture::LoadTexture(const std::string filename)
{
	std::string path = "";
#if defined(__vita__)
	path += "ux0:";
#endif
	path += filename;

	std::string debugText = "Loading texture: ";
	debugText += filename;
	Debug::Print(debugText);

	// Load image with stb_image
	// stbi_set_flip_vertically_on_load(GL_TRUE);
	unsigned char* buffer = stbi_load(path.c_str(), &width, &height,
		&nrChannels, 4);
	if (!buffer)
	{
		debugText = "Failed to load texture";
		Debug::PrintError(debugText);
		return;
	}
#if defined(__PSP__)
	SetData(buffer);
#endif

#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	textureId = Engine::renderer->CreateNewTexture();
	Engine::renderer->BindTexture(this);
	unsigned int rgba = 0x1908;
	Engine::renderer->SetTextureData(this, rgba, buffer);
#endif

	stbi_image_free(buffer);
	isValid = true;
	Debug::Print("Texture loaded");
}

void Texture::Unload()
{
	Engine::renderer->DeleteTexture(this);
}

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

Texture::Filter Texture::GetFilter() const
{
	return filter;
}

Texture::WrapMode Texture::GetWrapMode() const
{
	return wrapMode;
}

#pragma endregion

/// <summary>
/// Update texture if the filter has changed
/// </summary>
void Texture::UpdateTextureFilter()
{
	// Engine::renderer->BindTexture(this);
	// Engine::renderer->SetTextureFilter(this, filter);
	// Engine::renderer->SetTextureWrapMode(wrapMode);
	// Engine::renderer->SetAnisotropyLevel(EngineSettings::anisotropicLevel);
}