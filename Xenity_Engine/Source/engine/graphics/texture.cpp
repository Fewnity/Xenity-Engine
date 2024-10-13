// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "texture.h"

#include <malloc.h>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>

#if defined(__vita__)
#include <vitaGL.h>
#elif defined(__PSP__)
#include <pspgu.h>
#include <psp/video_hardware_dxtn.h>
#include <pspkernel.h>
#include <vram.h>
#elif defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
#include <thread>
#include <glad/glad.h>
#elif defined(_EE)
// #include "renderer/renderer_gskit.h"
#include <graph.h>
#include <dma.h>
#include <kernel.h>
#include <gs_psm.h>
#include "renderer/renderer_vu1.h"
#elif defined(__PS3__)
#include <rsx/rsx.h>
#endif

#include <engine/engine.h>
#include <engine/debug/debug.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/file_system/file.h>
#include <engine/tools/math.h>
#include <engine/file_system/async_file_loading.h>
#include <engine/debug/memory_tracker.h>
#include <engine/asset_management/project_manager.h>
#include <engine/debug/performance.h>
#include <engine/debug/stack_debug_object.h>

#include "renderer/renderer.h"

Texture::Texture()
{
	TextureSettingsStandalone* textureSettingsStandalone = new TextureSettingsStandalone();
	TextureSettingsPSP* textureSettingsPSP = new TextureSettingsPSP();
	TextureSettingsPSVITA* textureSettingsPSVITA = new TextureSettingsPSVITA();
	TextureSettingsPS3* textureSettingsPS3 = new TextureSettingsPS3();

	m_settings.push_back(textureSettingsStandalone);
	m_settings.push_back(textureSettingsPSP);
	m_settings.push_back(textureSettingsPSVITA);
	m_settings.push_back(textureSettingsPS3);
}

ReflectiveData Texture::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

ReflectiveData Texture::GetMetaReflectiveData(AssetPlatform platform)
{
	ReflectiveData reflectedVariables;
	ReflectiveData reflectedVariablesPlatform = m_settings[static_cast<int>(platform)]->GetReflectiveData();
	reflectedVariables.insert(reflectedVariables.end(), reflectedVariablesPlatform.begin(), reflectedVariablesPlatform.end());
	return reflectedVariables;
}

void Texture::OnReflectionUpdated()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

#if defined(EDITOR)
	if(previousResolution != GetCookResolution() && m_fileStatus == FileStatus::FileStatus_Loaded && isValid)
	{
		previousResolution = GetCookResolution();
		UnloadFileReference();
		LoadFileReference();
	}
#endif
}

std::shared_ptr<Texture> Texture::MakeTexture()
{
	std::shared_ptr<Texture> newTexture = std::make_shared<Texture>();
	AssetManager::AddFileReference(newTexture);
	return newTexture;
}

Texture::~Texture()
{
	Debug::Print("Texture::~Texture()" + std::to_string(textureId), true);
	this->UnloadFileReference();
}

void Texture::LoadFileReference()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	if (m_fileStatus == FileStatus::FileStatus_Not_Loaded)
	{
		m_fileStatus = FileStatus::FileStatus_Loading;

#if defined(EDITOR)
		AsyncFileLoading::AddFile(shared_from_this());

		std::thread threadLoading = std::thread(&Texture::CreateTexture, this, GetFilter(), GetUseMipmap());
		threadLoading.detach();
#else
		CreateTexture(GetFilter(), GetUseMipmap());
		OnLoadFileReferenceFinished();
#endif
	}
}

void Texture::UnloadFileReference()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	if (Engine::IsRunning(true))
	{
		if (m_fileStatus == FileStatus::FileStatus_Loaded)
		{
			m_fileStatus = FileStatus::FileStatus_Not_Loaded;
			Unload();
		}
	}
}

void Texture::ClearSpriteSelections()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	const size_t selectionCount = spriteSelections.size();
	for (size_t i = 0; i < selectionCount; i++)
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
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	SetFilter(filter);

	m_settings[static_cast<int>(Application::GetAssetPlatform())]->useMipMap = useMipMap;

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
void copy_texture_data(void *dest, const void *src, int width, int height, const PSPTextureType destType, const PSPTextureType srcType)
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
		if (srcType == PSPTextureType::RGBA_4444 || srcType == PSPTextureType::RGBA_5650 || srcType == PSPTextureType::RGBA_5551)
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
		if (srcType == PSPTextureType::RGBA_8888 && destType == PSPTextureType::RGBA_4444)
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
		else if (srcType == PSPTextureType::RGBA_8888 && destType == PSPTextureType::RGBA_5650)
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
		else if (srcType == PSPTextureType::RGBA_8888 && destType == PSPTextureType::RGBA_5551)
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

unsigned int GetColorByteCount(PSPTextureType psm)
{
	switch (psm)
	{
	//case GU_PSM_T4:
	//	return 0; // 0.5?

	//case GU_PSM_T8:
	//	return 1;

	case PSPTextureType::RGBA_5650:
	case PSPTextureType::RGBA_5551:
	case PSPTextureType::RGBA_4444:
	//case GU_PSM_T16:
		return 2;

	case PSPTextureType::RGBA_8888:
	/*case GU_PSM_T32:
	case GU_PSM_DXT1:
	case GU_PSM_DXT3:
	case GU_PSM_DXT5:*/
		return 4;

	default:
		return 0;
	}
	
	//switch (psm)
	//{
	//case GU_PSM_T4:
	//	return 0; // 0.5?

	//case GU_PSM_T8:
	//	return 1;

	//case GU_PSM_5650:
	//case GU_PSM_5551:
	//case GU_PSM_4444:
	//case GU_PSM_T16:
	//	return 2;

	//case GU_PSM_8888:
	//case GU_PSM_T32:
	//case GU_PSM_DXT1:
	//case GU_PSM_DXT3:
	//case GU_PSM_DXT5:
	//	return 4;

	//default:
	//	return 0;
	//}
}

void Texture::SetTextureLevel(int level, const unsigned char *texData)
{
	XASSERT(texData != nullptr, "[Texture::SetTextureLevel] texData is nullptr");

	PSPTextureType type = reinterpret_cast<TextureSettingsPSP*>(m_settings[static_cast<int>(Application::GetAssetPlatform())])->type;
	bool tryPutInVram = reinterpret_cast<TextureSettingsPSP*>(m_settings[static_cast<int>(Application::GetAssetPlatform())])->tryPutInVram;

	bool needResize = false;
	int bytePerPixel = GetColorByteCount(type);

	int diviser = (int)pow(2, level);

	int resizedPW = pW / diviser;
	int resizedPH = pH / diviser;

	int byteCount = (resizedPW * resizedPH) * bytePerPixel;

	unsigned int *dataBuffer = (unsigned int *)memalign(16, byteCount);

	if (level != 0 || (m_width != pW || height != pH))
	{
		needResize = true;
	}

	if (needResize)
	{
		unsigned char *resizedData = (unsigned char *)malloc((resizedPW * resizedPH) * 4);
		stbir_resize_uint8(texData, m_width, height, 0, resizedData, resizedPW, resizedPH, 0, 4);
		copy_texture_data(dataBuffer, resizedData, resizedPW, resizedPH, type, PSPTextureType::RGBA_8888);
		free(resizedData);
	}
	else
	{
		copy_texture_data(dataBuffer, texData, pW, pH, type, PSPTextureType::RGBA_8888);
	}

	bool isLevelInVram = true;
	if (resizedPW > 256 || resizedPH > 256) 
	{
		isLevelInVram = false;
		Debug::PrintWarning("Texture too big to be in vram", true);
	}

	// Allocate memory in ram or vram
	if (isLevelInVram)
	{
		unsigned int* newData = nullptr;
		if(tryPutInVram)
		{
			newData = (unsigned int*)vramalloc(byteCount);
		}

		// If there is no more free vram or if we don't want to put it in vram
		if (!newData)
		{
			if(tryPutInVram)
			{
				Debug::PrintWarning("No enough free vram", true);
			}
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
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

#if defined(__vita__) || defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	textureId = Engine::GetRenderer().CreateNewTexture();
	Engine::GetRenderer().BindTexture(*this);
	const unsigned int rgba = 0x1908;
	Engine::GetRenderer().SetTextureData(*this, rgba, m_buffer);
#endif

	free(m_buffer);
	isValid = true;
}

void Texture::SetData(const unsigned char *texData)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	XASSERT(texData != nullptr, "[Texture::SetTextureLevel] texData is nullptr");
	// sceGeEdramSetSize(4096);
	// The psp needs a pow2 sized texture
#if defined(__PSP__)
	// type = GU_PSM_DXT5;
	// type = GU_PSM_DXT3;
	// type = GU_PSM_DXT1;

	// Get pow2 size
	pW = Math::nextPow2(m_width);
	pH = Math::nextPow2(height);

	SetTextureLevel(0, texData);
	if (GetUseMipmap())
	{
		SetTextureLevel(1, texData);
		// SetTextureLevel(2, texData);
		// SetTextureLevel(3, texData);
		m_settings[static_cast<int>(Application::GetAssetPlatform())]->mipmaplevelCount = 1;
	}
#elif defined(__PS3__)
	// m_ps3buffer = (unsigned char*)rsxMemalign(128, (GetWidth() * GetHeight() * 4));
	// if(!m_ps3buffer) 
	//     return;
		
	// unsigned char* upBuffer = m_ps3buffer;
	// for(int i=0; i< GetWidth() * GetHeight() * 4; i+=4)
	// {
	// 	upBuffer[i + 0] = m_buffer[(i + 3)];
    //     upBuffer[i + 1] = m_buffer[(i + 0)];
	// 	upBuffer[i + 2] = m_buffer[(i + 1)];
	// 	upBuffer[i + 3] = m_buffer[(i + 2)];
	// }

#elif defined(_EE)
	texbuff.width = m_width;
	texbuff.psm = GS_PSM_32;
	texbuff.address = graph_vram_allocate(m_width, height, GS_PSM_32, GRAPH_ALIGN_BLOCK);

	texbuff.info.width = draw_log2(m_width);
	texbuff.info.height = draw_log2(height);
	texbuff.info.components = TEXTURE_COMPONENTS_RGB;
	texbuff.info.function = TEXTURE_FUNCTION_MODULATE;

	packet2_t *packet2 = packet2_create(50, P2_TYPE_NORMAL, P2_MODE_CHAIN, 0);
	packet2_update(packet2, draw_texture_transfer(packet2->next, (void *)texData, m_width, height, GS_PSM_32, texbuff.address, texbuff.width));
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

#if defined(__vita__) || defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	textureId = Engine::GetRenderer().CreateNewTexture();
	Engine::GetRenderer().BindTexture(*this);
	// unsigned int alpha = 0x1906;

	Engine::GetRenderer().SetTextureData(*this, GL_LUMINANCE_ALPHA, texData);
#endif

	isValid = true;
}

void Texture::LoadTexture()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);
	
	bool openResult = true;
#if defined(EDITOR)
	openResult = m_file->Open(FileMode::ReadOnly);
#endif
	if (openResult)
	{
		size_t fileBufferSize = m_fileSize;
		unsigned char* fileData = nullptr;
#if defined(EDITOR)
		fileData = m_file->ReadAllBinary(fileBufferSize);
		m_file->Close();
#else
		fileData = ProjectManager::fileDataBase.GetBitFile().ReadBinary(m_filePosition, fileBufferSize);
#endif

		// Only for editor, live resizing
#if defined(EDITOR)
		// Load image with stb_image
		unsigned char* data2 = stbi_load_from_memory(fileData, fileBufferSize, &m_width, &height,
									   &nrChannels, 4);

		free(fileData);
		int newWidth = m_width;
		int newHeight = height;
		const int cookResolution = static_cast<int>(GetCookResolution());
		if((newWidth > height) && newWidth > cookResolution)
		{
			newHeight = static_cast<int>(height * (static_cast<float>(GetCookResolution()) / static_cast<float>(m_width)));
			newWidth = cookResolution;
		}
		else if((newHeight > m_width) && newHeight > cookResolution)
		{
			newWidth = static_cast<int>(m_width * (static_cast<float>(GetCookResolution()) / static_cast<float>(height)));
			newHeight = cookResolution;
		}
		else if ((newWidth == newHeight) && newWidth > cookResolution)
		{
			newWidth = cookResolution;
			newHeight = cookResolution;
		}

		m_buffer = (unsigned char*)malloc(newWidth * newHeight * 4);
		stbir_resize_uint8(data2, m_width, height, 0, m_buffer, newWidth, newHeight, 0, 4);
		free(data2);
		m_width = newWidth;
		height = newHeight;
#else
		// Load image with stb_image
		m_buffer = stbi_load_from_memory(fileData, fileBufferSize, &m_width, &height,
			&nrChannels, 4);

		free(fileData);
#endif

		if (!m_buffer)
		{
			Debug::PrintError("[Texture::LoadTexture] Failed to load texture", true);
			m_fileStatus = FileStatus::FileStatus_Failed;
			return;
		}

#if defined(__PSP__) || defined(_EE) || defined(__PS3__)
		SetData(m_buffer);
#else
#if defined (DEBUG)
		Performance::s_textureMemoryTracker->Allocate(m_width * height * 4);
#endif
#endif

	}
	else
	{
		Debug::PrintError("[Texture::LoadTexture] Failed to open texture file", true);
		m_fileStatus = FileStatus::FileStatus_Failed;
	}
	m_fileStatus = FileStatus::FileStatus_Loaded;
}

void Texture::Unload()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	ClearSpriteSelections();
	Engine::GetRenderer().DeleteTexture(*this);

#if defined (DEBUG)
	Performance::s_textureMemoryTracker->Deallocate(m_width * height * 4);
#endif
}

#pragma endregion