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
#include "../../psp/video_hardware_dxtn.h"
#include <pspkernel.h>
#include <vram.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <thread>
#include <glad/glad.h>
#endif

Texture::Texture()
{
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
		//SetData(data);
		isValid = true;
	}
}

std::unordered_map<std::string, ReflectionEntry> Texture::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	return reflectedVariables;
}

std::unordered_map<std::string, ReflectionEntry> Texture::GetMetaReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	Reflection::AddReflectionVariable(reflectedVariables, inVram, "inVram", true);
	Reflection::AddReflectionVariable(reflectedVariables, useMipMap, "useMipMap", true);
	Reflection::AddReflectionVariable(reflectedVariables, mipmaplevelCount, "mipmaplevelCount", true);
	Reflection::AddReflectionVariable(reflectedVariables, (int&)filter, "filter", true);
	Reflection::AddReflectionVariable(reflectedVariables, (int&)wrapMode, "wrapMode", true);
	return reflectedVariables;
}

std::shared_ptr<Texture> Texture::MakeTexture()
{
	std::shared_ptr<Texture> newTexture = std::make_shared<Texture>();
	AssetManager::AddFileReference(newTexture);
	return newTexture;
}

std::shared_ptr<Texture> Texture::MakeTexture(unsigned char* data, const int channelCount, const int width, const int height, bool loadInVram)
{
	std::shared_ptr<Texture> newTexture = std::make_shared<Texture>(data, channelCount, width, height, loadInVram);
	AssetManager::AddFileReference(newTexture);
	return newTexture;
}

Texture::~Texture()
{
	Debug::Print("Texture::~Texture()" + std::to_string(textureId));
	this->UnloadFileReference();
}

void Texture::LoadFileReference()
{
	if (!isLoaded)
	{
		isLoaded = true;
#if defined(EDITOR)
		isLoading = true;
		Engine::threadLoadingMutex.lock();
		Engine::threadLoadedFiles.push_back(shared_from_this());
		Engine::threadLoadingMutex.unlock();

		std::thread threadLoading = std::thread(&Texture::CreateTexture, this, filter, useMipMap);
		threadLoading.detach();
#else
		CreateTexture(filter, useMipMap);
		OnLoadFileReferenceFinished();
#endif
	}
}

void Texture::UnloadFileReference()
{
	if (Engine::IsRunning())
	{
		Debug::Print("Unload");
		if (isLoaded)
		{
			isLoaded = false;
			Unload();
		}
	}
}

/// <summary>
/// Create the texture from the file path and texture settings
/// </summary>
/// <param name="filePath">File path</param>
/// <param name="filter">Filter to use</param>
/// <param name="useMipMap">Will texture use mipmap</param>
void Texture::CreateTexture(const Filter filter, const bool useMipMap)
{
	this->filter = filter;
	this->useMipMap = useMipMap;

	LoadTexture();
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
	/*for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			((unsigned int*)dest)[x + y * width] = ((unsigned int*)src)[x + y * width];
		}
	}*/
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
		unsigned char* resizedData = (unsigned char*)malloc((resizedPW * resizedPH) * 4);
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
		unsigned int* newData = (unsigned int*)vramalloc(byteCount); // Divide by 8 when dxt1, by 4 when dxt3 and dxt5
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
	//tx_compress_dxtn(4, resizedPW, resizedPH, (const unsigned char*)dataBuffer, type, (unsigned char*)data[level]);

	// Place image data in the memory
	swizzle_fast((u8*)data[level], (const u8*)dataBuffer, resizedPW * bytePerPixel, resizedPH);
	//copy_texture_data(data[level], dataBuffer, pW, pH, type, type);
	free(dataBuffer);
	sceKernelDcacheWritebackInvalidateAll();
}


#endif

void Texture::OnLoadFileReferenceFinished()
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	textureId = Engine::renderer->CreateNewTexture();
	Engine::renderer->BindTexture(GetThisShared());
	unsigned int rgba = 0x1908;
	Engine::renderer->SetTextureData(GetThisShared(), rgba, buffer);
#endif

	stbi_image_free(buffer);
	isValid = true;
}

void Texture::SetData(const unsigned char* texData)
{
	// sceGeEdramSetSize(4096);
	// The psp needs a pow2 sized texture
#if defined(__PSP__)
	//type = GU_PSM_8888;
	//type = GU_PSM_4444;
	//type = GU_PSM_5650;
	type = GU_PSM_5551;
	//type = GU_PSM_DXT5;
	//type = GU_PSM_DXT3;
	//type = GU_PSM_DXT1;

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
	Engine::renderer->BindTexture(GetThisShared());
	//unsigned int alpha = 0x1906;

	Engine::renderer->SetTextureData(GetThisShared(), GL_LUMINANCE_ALPHA, texData);
#endif
}

void Texture::LoadTexture()
{
	Debug::Print("Loading texture: " + file->GetPath());

	int fileBufferSize = 0;
	bool openResult = file->Open(false);
	if (openResult)
	{
		unsigned char* fileData = file->ReadAllBinary(fileBufferSize);
		file->Close();

		// Load image with stb_image
		// stbi_set_flip_vertically_on_load(GL_TRUE);
		buffer = stbi_load_from_memory(fileData, fileBufferSize, &width, &height,
			&nrChannels, 4);

		free(fileData);

		if (!buffer)
		{
			Debug::PrintError("[Texture::LoadTexture] Failed to load texture");
			return;
		}
#if defined(__PSP__)
		SetData(buffer);
#endif
	}
	else
	{
		Debug::PrintError("[Texture::LoadTexture] Failed to open texture file");
	}
	isLoading = false;
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