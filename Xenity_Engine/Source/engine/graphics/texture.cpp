#include "texture.h"
#include "renderer/renderer.h"

#include <engine/engine.h>
#include <engine/debug/debug.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/file_system/file.h>
#include <engine/tools/math.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>
#include <malloc.h>
#include <string>

#if defined(__vita__)
#include <vitaGL.h>
#elif defined(__PSP__)
#include <pspgu.h>
#include <psp/video_hardware_dxtn.h>
#include <pspkernel.h>
#include <vram.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <thread>
#include <glad/glad.h>
#elif defined(_EE)
// #include "renderer/renderer_gskit.h"
#include <graph.h>
#include <dma.h>
#include <kernel.h>
#include <gs_psm.h>
#include "renderer/renderer_vu1.h"
#endif
#include <engine/file_system/async_file_loading.h>

Texture::Texture()
{
}

ReflectiveData Texture::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

ReflectiveData Texture::GetMetaReflectiveData()
{
	ReflectiveData reflectedVariables;
	//Reflective::AddVariable(reflectedVariables, inVram, "inVram", true);
	Reflective::AddVariable(reflectedVariables, useMipMap, "useMipMap", true);
	Reflective::AddVariable(reflectedVariables, mipmaplevelCount, "mipmaplevelCount", true);
	Reflective::AddVariable(reflectedVariables, filter, "filter", true);
	Reflective::AddVariable(reflectedVariables, wrapMode, "wrapMode", true);
	Reflective::AddVariable(reflectedVariables, spriteSelections, "spriteSelections", true);
	return reflectedVariables;
}

std::shared_ptr<Texture> Texture::MakeTexture()
{
	std::shared_ptr<Texture> newTexture = std::make_shared<Texture>();
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
		AsyncFileLoading::AddFile(shared_from_this());

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
	if (Engine::IsRunning(true))
	{
		Debug::Print("Unload");
		if (isLoaded)
		{
			isLoaded = false;
			Unload();
		}
	}
}

void Texture::ClearSpriteSelections()
{
	int selectionCount = spriteSelections.size();
	for (int i = 0; i < selectionCount; i++)
	{
		delete spriteSelections[i];
	}
	spriteSelections.clear();
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
void swizzle_fast(u8 *out, const u8 *in, const unsigned int width, const unsigned int height)
{
	unsigned int blockx, blocky;
	unsigned int j;

	unsigned int width_blocks = (width / 16);
	unsigned int height_blocks = (height / 8);

	unsigned int src_pitch = (width - 16) / 4;
	unsigned int src_row = width * 8;

	const u8 *ysrc = in;
	u32 *dst = (u32 *)out;

	for (blocky = 0; blocky < height_blocks; ++blocky)
	{
		const u8 *xsrc = ysrc;
		for (blockx = 0; blockx < width_blocks; ++blockx)
		{
			const u32 *src = (u32 *)xsrc;
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

// See https://github.com/pspdev/pspsdk/blob/master/src/debug///scr_printf.c
void copy_texture_data(void *dest, const void *src, int width, int height, const int destType, const int srcType)
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
				((unsigned int *)dest)[x + y * width] = ((unsigned int *)src)[x + y * width];
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
					uint32_t srcPixel = ((uint32_t *)src)[x + y * width];

					uint16_t r = (srcPixel >> 24) & 0xFF;
					uint16_t g = (srcPixel >> 16) & 0xFF;
					uint16_t b = (srcPixel >> 8) & 0xFF;
					uint16_t a = srcPixel & 0xFF;

					uint16_t destPixel = (r >> 4) << 12 | (g >> 4) << 8 | (b >> 4) << 4 | (a >> 4);

					((uint16_t *)dest)[x + y * width] = destPixel;
				}
			}
		}
		else if (srcType == GU_PSM_8888 && destType == GU_PSM_5650)
		{
			for (unsigned int y = 0; y < height; y++)
			{
				for (unsigned int x = 0; x < width; x++)
				{
					uint32_t srcPixel = ((uint32_t *)src)[x + y * width];

					uint16_t r = (srcPixel >> 19) & 0x1F;
					uint16_t g = (srcPixel >> 10) & 0x3F;
					uint16_t b = (srcPixel >> 3) & 0x1F;

					uint16_t destPixel = b | (g << 5) | (r << 11);

					((uint16_t *)dest)[x + y * width] = destPixel;
				}
			}
		}
		else if (srcType == GU_PSM_8888 && destType == GU_PSM_5551)
		{
			for (unsigned int y = 0; y < height; y++)
			{
				for (unsigned int x = 0; x < width; x++)
				{
					uint32_t srcPixel = ((uint32_t *)src)[x + y * width];

					uint16_t a = (srcPixel >> 24) ? 0x8000 : 0;
					uint16_t b = (srcPixel >> 19) & 0x1F;
					uint16_t g = (srcPixel >> 11) & 0x1F;
					uint16_t r = (srcPixel >> 3) & 0x1F;

					uint16_t destPixel = a | r | (g << 5) | (b << 10);

					((uint16_t *)dest)[x + y * width] = destPixel;
				}
			}
		}
	}
}

unsigned int GetColorByteCount(unsigned int psm)
{
	switch (psm)
	{
	case GU_PSM_T4:
		return 0; // 0.5?

	case GU_PSM_T8:
		return 1;

	case GU_PSM_5650:
	case GU_PSM_5551:
	case GU_PSM_4444:
	case GU_PSM_T16:
		return 2;

	case GU_PSM_8888:
	case GU_PSM_T32:
	case GU_PSM_DXT1:
	case GU_PSM_DXT3:
	case GU_PSM_DXT5:
		return 4;

	default:
		return 0;
	}
}

void Texture::SetTextureLevel(int level, const unsigned char *texData)
{
	bool needResize = false;
	int bytePerPixel = GetColorByteCount(type);

	int diviser = (int)pow(2, level);

	int resizedPW = pW / diviser;
	int resizedPH = pH / diviser;

	int byteCount = (resizedPW * resizedPH) * bytePerPixel;

	unsigned int *dataBuffer = (unsigned int *)memalign(16, byteCount);

	if (level != 0 || (width != pW || height != pH))
	{
		needResize = true;
	}

	if (needResize)
	{
		unsigned char *resizedData = (unsigned char *)malloc((resizedPW * resizedPH) * 4);
		stbir_resize_uint8(texData, width, height, 0, resizedData, resizedPW, resizedPH, 0, 4);
		copy_texture_data(dataBuffer, resizedData, resizedPW, resizedPH, type, GU_PSM_8888);
		free(resizedData);
	}
	else
	{
		copy_texture_data(dataBuffer, texData, pW, pH, type, GU_PSM_8888);
	}

	bool isLevelInVram = true;
	if (resizedPW > 256 || resizedPH > 256)
		isLevelInVram = false;

	// Allocate memory in ram or vram
	if (isLevelInVram)
	{
		unsigned int *newData = (unsigned int *)vramalloc(byteCount); // Divide by 8 when dxt1, by 4 when dxt3 and dxt5
		// If there is no more free vram
		if (!newData)
		{
			Debug::PrintWarning("No more free vram");
			newData = (unsigned int *)memalign(16, byteCount);
			isLevelInVram = false;
		}
		data.push_back((unsigned int *)newData);
	}
	else
	{
		data.push_back((unsigned int *)memalign(16, byteCount));
	}
	inVram.push_back(isLevelInVram);
	// tx_compress_dxtn(4, resizedPW, resizedPH, (const unsigned char*)dataBuffer, type, (unsigned char*)data[level]);

	// Place image data in the memory
	swizzle_fast((u8 *)data[level], (const u8 *)dataBuffer, resizedPW * bytePerPixel, resizedPH);
	// copy_texture_data(data[level], dataBuffer, pW, pH, type, type);
	free(dataBuffer);
	sceKernelDcacheWritebackInvalidateAll();
}

#endif

void Texture::OnLoadFileReferenceFinished()
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	textureId = Engine::GetRenderer().CreateNewTexture();
	Engine::GetRenderer().BindTexture(GetThisShared());
	const unsigned int rgba = 0x1908;
	Engine::GetRenderer().SetTextureData(GetThisShared(), rgba, buffer);
#endif

	stbi_image_free(buffer);
	isValid = true;
}

void Texture::SetData(const unsigned char *texData)
{
	// sceGeEdramSetSize(4096);
	// The psp needs a pow2 sized texture
#if defined(__PSP__)
	// type = GU_PSM_8888;
	// type = GU_PSM_4444;
	// type = GU_PSM_5650;
	type = GU_PSM_5551;
	// type = GU_PSM_DXT5;
	// type = GU_PSM_DXT3;
	// type = GU_PSM_DXT1;

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
#elif defined(_EE)
	texbuff.width = width;
	texbuff.psm = GS_PSM_32;
	texbuff.address = graph_vram_allocate(width, height, GS_PSM_32, GRAPH_ALIGN_BLOCK);

	texbuff.info.width = draw_log2(width);
	texbuff.info.height = draw_log2(height);
	texbuff.info.components = TEXTURE_COMPONENTS_RGB;
	texbuff.info.function = TEXTURE_FUNCTION_MODULATE;

	packet2_t *packet2 = packet2_create(50, P2_TYPE_NORMAL, P2_MODE_CHAIN, 0);
	packet2_update(packet2, draw_texture_transfer(packet2->next, (void *)texData, width, height, GS_PSM_32, texbuff.address, texbuff.width));
	packet2_update(packet2, draw_texture_flush(packet2->next));
	FlushCache(0);
	dma_channel_send_packet2(packet2, DMA_CHANNEL_GIF, 1);
	dma_wait_fast();
	packet2_free(packet2);

	// for (int i = 0; i < width * height; i++)
	// {
	// 	texbuff.address[i] = 0;
	// }

	// ps2Tex.Width = width;
	// ps2Tex.Height = height;
	// ps2Tex.PSM = GS_PSM_CT32;
	// ps2Tex.VramClut = 0;
	// ps2Tex.Clut = NULL;

	// ps2Tex.Filter = GS_FILTER_NEAREST;
	// ps2Tex.Mem = (u32 *)memalign(128, ps2Tex.Width * ps2Tex.Height * 4);

	// struct pixel
	// {
	// 	u8 r, g, b, a;
	// };
	// struct pixel *Pixels = (struct pixel *)ps2Tex.Mem;
	// for (size_t i = 0; i < ps2Tex.Width * ps2Tex.Height; i++)
	// {
	// 	Pixels[i].r = texData[i * 4 + 0];
	// 	Pixels[i].g = texData[i * 4 + 1];
	// 	Pixels[i].b = texData[i * 4 + 2];
	// 	Pixels[i].a = 255 - texData[i * 4 + 3];
	// }
	// // Tex.Vram = gsKit_vram_alloc(gsGlobal, gsKit_texture_size(Tex2.Width, Tex2.Height, Tex2.PSM), GSKIT_ALLOC_USERBUFFER);
	// //  if (!Tex2.Vram)
	// //  	printf("No more Vram");
	// gsKit_TexManager_bind(((RendererGsKit &)Engine::GetRenderer()).gsGlobal, &ps2Tex);
#endif

#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	textureId = Engine::GetRenderer().CreateNewTexture();
	Engine::GetRenderer().BindTexture(GetThisShared());
	// unsigned int alpha = 0x1906;

	Engine::GetRenderer().SetTextureData(GetThisShared(), GL_LUMINANCE_ALPHA, texData);
#endif

	isValid = true;
}

void Texture::LoadTexture()
{
	Debug::Print("Loading texture: " + file->GetPath());

	const bool openResult = file->Open(FileMode::ReadOnly);
	if (openResult)
	{
		int fileBufferSize = 0;
		unsigned char *fileData = file->ReadAllBinary(fileBufferSize);
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
#if defined(__PSP__) || defined(_EE)
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
	ClearSpriteSelections();
	Engine::GetRenderer().DeleteTexture(this);
}

/// <summary>
/// Set texture's filter
/// </summary>
/// <param name="filter"></param>
void Texture::SetFilter(const Filter filter)
{
	this->filter = filter;
}

void Texture::SetWrapMode(const WrapMode mode)
{
	wrapMode = mode;
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

Filter Texture::GetFilter() const
{
	return filter;
}

WrapMode Texture::GetWrapMode() const
{
	return wrapMode;
}

#pragma endregion