// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(__PSP__)

#include "texture_psp.h"

#include <malloc.h>
#include <string>

#include <pspgu.h>
#include <psp/video_hardware_dxtn.h>
#include <pspkernel.h>
#include <vram.h>

//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
//#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>

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

TexturePSP::TexturePSP()
{
}

TexturePSP::~TexturePSP()
{
	Debug::Print("TexturePSP::~TextureDefault()" + std::to_string(textureId), true);
	this->UnloadFileReference();
}

void TexturePSP::OnLoadFileReferenceFinished()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	SetData(m_buffer);

	free(m_buffer);
	isValid = true;
}

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

// See https://github.com/pspdev/pspsdk/blob/master/src/debug///scr_printf.c
void copy_texture_data(void* dest, const void* src, int width, int height, const PSPTextureType destType, const PSPTextureType srcType)
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
				((unsigned int*)dest)[x + y * width] = ((unsigned int*)src)[x + y * width];
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
		else if (srcType == PSPTextureType::RGBA_8888 && destType == PSPTextureType::RGBA_5650)
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
		else if (srcType == PSPTextureType::RGBA_8888 && destType == PSPTextureType::RGBA_5551)
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

void TexturePSP::SetTextureLevel(int level, const unsigned char* texData)
{
	XASSERT(texData != nullptr, "[TexturePSP::SetTextureLevel] texData is nullptr");

	PSPTextureType type = reinterpret_cast<TextureSettingsPSP*>(m_settings[static_cast<int>(Application::GetAssetPlatform())])->type;
	bool tryPutInVram = reinterpret_cast<TextureSettingsPSP*>(m_settings[static_cast<int>(Application::GetAssetPlatform())])->tryPutInVram;

	bool needResize = false;
	int bytePerPixel = GetColorByteCount(type);

	int diviser = (int)pow(2, level);

	int resizedPW = pW / diviser;
	int resizedPH = pH / diviser;

	int byteCount = (resizedPW * resizedPH) * bytePerPixel;

	unsigned int* dataBuffer = (unsigned int*)memalign(16, byteCount);

	if (level != 0 || (m_width != pW || height != pH))
	{
		needResize = true;
	}

	if (needResize)
	{
		unsigned char* resizedData = (unsigned char*)malloc((resizedPW * resizedPH) * 4);
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
		if (tryPutInVram)
		{
			newData = (unsigned int*)vramalloc(byteCount);
		}

		// If there is no more free vram or if we don't want to put it in vram
		if (!newData)
		{
			if (tryPutInVram)
			{
				Debug::PrintWarning("No enough free vram", true);
			}
			newData = (unsigned int*)memalign(16, byteCount);
			isLevelInVram = false;
		}
		data.push_back((unsigned int*)newData);
	}
	else
	{
		data.push_back((unsigned int*)memalign(16, byteCount));
	}
	inVram.push_back(isLevelInVram);
	// tx_compress_dxtn(4, resizedPW, resizedPH, (const unsigned char*)dataBuffer, type, (unsigned char*)data[level]);

	// Place image data in the memory
	swizzle_fast((u8*)data[level], (const u8*)dataBuffer, resizedPW * bytePerPixel, resizedPH);
	// copy_texture_data(data[level], dataBuffer, pW, pH, type, type);
	free(dataBuffer);
	sceKernelDcacheWritebackInvalidateAll();
}

void TexturePSP::SetData(const unsigned char* texData)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	XASSERT(texData != nullptr, "[TexturePSP::SetTextureLevel] texData is nullptr");
	// sceGeEdramSetSize(4096);
	// The psp needs a pow2 sized texture
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

	isValid = true;
}

#endif