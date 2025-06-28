// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine
#if defined(__PS3__)
#include "texture_ps3.h"

#include <malloc.h>
#include <string>

#include <stb_image.h>
#include <stb_image_resize.h>

#include <engine/engine.h>
#include <engine/debug/debug.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/file_system/file.h>
#include <engine/tools/internal_math.h>
#include <engine/file_system/async_file_loading.h>
#include <engine/debug/memory_tracker.h>
#include <engine/asset_management/project_manager.h>
#include <engine/debug/performance.h>
#include <engine/debug/stack_debug_object.h>
#include <engine/graphics/renderer/renderer_rsx.h>
#include <engine/graphics/shader/shader_rsx.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/renderer/renderer.h>

TexturePS3::TexturePS3()
{
}

TexturePS3::~TexturePS3()
{
	this->UnloadFileReference();
}

void TexturePS3::OnLoadFileReferenceFinished()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	SetData(m_buffer);

	free(m_buffer);
	isValid = true;
}

unsigned int TexturePS3::GetColorByteCount(PS3TextureType type)
{
	switch (type)
	{
	case PS3TextureType::ARGB_0565:
	case PS3TextureType::ARGB_1555:
	case PS3TextureType::ARGB_4444:
		return 2;

	case PS3TextureType::ARGB_8888:
		return 4;

	default:
		return 0;
	}
}

unsigned int TexturePS3::TextureTypeToGCMType(PS3TextureType type)
{
	switch (type)
	{
	case PS3TextureType::ARGB_0565:
		return GCM_TEXTURE_FORMAT_R5G6B5;
	case PS3TextureType::ARGB_1555:
		return GCM_TEXTURE_FORMAT_A1R5G5B5;
	case PS3TextureType::ARGB_4444:
		return GCM_TEXTURE_FORMAT_A4R4G4B4;
	case PS3TextureType::ARGB_8888:
		return GCM_TEXTURE_FORMAT_A8R8G8B8;
	}
}

static void ConvertTextureDataType(void* dest, const void* src, unsigned int width, unsigned int height, const PS3TextureType destType, const PS3TextureType srcType)
{
	if (destType == srcType)
	{
		if (srcType == PS3TextureType::ARGB_4444 || srcType == PS3TextureType::ARGB_0565 || srcType == PS3TextureType::ARGB_1555)
		{
			height /= 2;
		}
		for (unsigned int y = 0; y < height; y++)
		{
			for (unsigned int x = 0; x < width; x++)
			{
				uint32_t srcPixel = ((uint32_t*)src)[x + y * width];
				uint8_t r = (srcPixel >> 24) & 0xFF;
				uint8_t g = (srcPixel >> 16) & 0xFF;
				uint8_t b = (srcPixel >> 8) & 0xFF;
				uint8_t a = srcPixel & 0xFF;

				uint32_t destPixel = (a) << 24 | (r) << 16 | (g) << 8 | (b);
				((uint32_t*)dest)[x + y * width] = destPixel;
			}
		}
	}
	else
	{
		if (srcType == PS3TextureType::ARGB_8888 && destType == PS3TextureType::ARGB_4444)
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

					uint16_t destPixel = (a >> 4) << 12 | (r >> 4) << 8 | (g >> 4) << 4 | (b >> 4);

					((uint16_t*)dest)[x + y * width] = destPixel;
				}
			}
		}
		else if (srcType == PS3TextureType::ARGB_8888 && destType == PS3TextureType::ARGB_0565)
		{
			for (unsigned int y = 0; y < height; y++)
			{
				for (unsigned int x = 0; x < width; x++)
				{
					uint32_t srcPixel = ((uint32_t*)src)[x + y * width];

					uint8_t r = (srcPixel >> 24) & 0xFF;
					uint8_t g = (srcPixel >> 16) & 0xFF;
					uint8_t b = (srcPixel >> 8) & 0xFF;

					uint16_t r5 = (r >> 3) & 0x1F;
					uint16_t g6 = (g >> 2) & 0x3F;
					uint16_t b5 = (b >> 3) & 0x1F;

					uint16_t destPixel = (r5 << 11) | (g6 << 5) | b5;

					((uint16_t*)dest)[x + y * width] = destPixel;
				}
			}
		}
		else if (srcType == PS3TextureType::ARGB_8888 && destType == PS3TextureType::ARGB_1555)
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

					uint16_t alpha = (a > 0) ? 1 : 0;
					uint16_t r5 = (r >> 3) & 0x1F;
					uint16_t g5 = (g >> 3) & 0x1F;
					uint16_t b5 = (b >> 3) & 0x1F;

					uint16_t destPixel = (alpha << 15) | (r5 << 10) | (g5 << 5) | b5;

					((uint16_t*)dest)[x + y * width] = destPixel;
				}
			}
		}
	}
}

void TexturePS3::SetData(const unsigned char* texData)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);
	XASSERT(texData != nullptr, "[TexturePS3::SetTextureLevel] texData is nullptr");

	if (!m_ps3buffer)
	{
		size_t byteCount = 0;
		if (!isFloatFormat)
		{
			byteCount = GetWidth() * GetHeight() * 4;
		}
		else if (isFloatFormat)
		{
			byteCount = GetWidth() * GetHeight() * 4 * sizeof(float);
		}
		m_ps3buffer = (unsigned char*)rsxMemalign(128, byteCount);
#if defined (DEBUG)
		Performance::s_textureMemoryTracker->Allocate(byteCount);
#endif
	}

	if (!m_ps3buffer)
	{
		return;
	}

	PS3TextureType type = GetSettings().type;
	unsigned int colorByteCount = GetColorByteCount(type);
	ConvertTextureDataType(m_ps3buffer, texData, GetWidth(), GetHeight(), type, PS3TextureType::ARGB_8888);

	rsxAddressToOffset(m_ps3buffer, &m_textureOffset);

	uint32_t resolutionMultiplier = 1;
	if (isFloatFormat)
	{
		resolutionMultiplier = 4;
		colorByteCount = 4;
	}

	if (!isFloatFormat)
	{
		m_gcmTexture.format = (TextureTypeToGCMType(type) | GCM_TEXTURE_FORMAT_LIN);
	}
	else
	{
		m_gcmTexture.format = (GCM_TEXTURE_FORMAT_W32_Z32_Y32_X32_FLOAT | GCM_TEXTURE_FORMAT_LIN | GCM_TEXTURE_FORMAT_UNRM);
	}
	m_gcmTexture.mipmap = 1;
	m_gcmTexture.dimension = GCM_TEXTURE_DIMS_2D;
	m_gcmTexture.cubemap = GCM_FALSE;
	m_gcmTexture.remap = ((GCM_TEXTURE_REMAP_TYPE_REMAP << GCM_TEXTURE_REMAP_TYPE_B_SHIFT) |
		(GCM_TEXTURE_REMAP_TYPE_REMAP << GCM_TEXTURE_REMAP_TYPE_G_SHIFT) |
		(GCM_TEXTURE_REMAP_TYPE_REMAP << GCM_TEXTURE_REMAP_TYPE_R_SHIFT) |
		(GCM_TEXTURE_REMAP_TYPE_REMAP << GCM_TEXTURE_REMAP_TYPE_A_SHIFT) |
		(GCM_TEXTURE_REMAP_COLOR_B << GCM_TEXTURE_REMAP_COLOR_B_SHIFT) |
		(GCM_TEXTURE_REMAP_COLOR_G << GCM_TEXTURE_REMAP_COLOR_G_SHIFT) |
		(GCM_TEXTURE_REMAP_COLOR_R << GCM_TEXTURE_REMAP_COLOR_R_SHIFT) |
		(GCM_TEXTURE_REMAP_COLOR_A << GCM_TEXTURE_REMAP_COLOR_A_SHIFT));
	m_gcmTexture.width = GetWidth() * resolutionMultiplier;
	m_gcmTexture.height = GetHeight() * resolutionMultiplier;
	m_gcmTexture.depth = 1;
	m_gcmTexture.location = GCM_LOCATION_RSX;
	m_gcmTexture.pitch = GetWidth() * colorByteCount * resolutionMultiplier;
	m_gcmTexture.offset = m_textureOffset;

	isValid = true;
	m_fileStatus = FileStatus::FileStatus_Loaded;
}

void TexturePS3::Bind() const
{
	if (!m_ps3buffer)
	{
		return;
	}

	gcmContextData* context = RendererRSX::context;
	rsxInvalidateTextureCache(context, GCM_INVALIDATE_TEXTURE);

	const ShaderRSX& rsxShader = dynamic_cast<const ShaderRSX&>(*Graphics::s_currentShader);

	uint8_t textureUnitIndex = rsxShader.m_textureUnit->index;
	if(isFloatFormat)
	{
		if (rsxShader.m_lightingDataTextureUnit)
		{
			textureUnitIndex = rsxShader.m_lightingDataTextureUnit->index;
		}
		else 
		{
			return;
		}
	}

	rsxLoadTexture(context, textureUnitIndex, &m_gcmTexture);
	rsxTextureControl(context, textureUnitIndex, GCM_TRUE, 0 << 8, 12 << 8, GCM_TEXTURE_MAX_ANISO_1);
	int minFilterValue = GCM_TEXTURE_LINEAR;
	int magfilterValue = GCM_TEXTURE_LINEAR;
	if (GetFilter() == Filter::Point)
	{
		minFilterValue = GCM_TEXTURE_NEAREST;
		magfilterValue = GCM_TEXTURE_NEAREST;
	}
	rsxTextureFilter(context, textureUnitIndex, 0, minFilterValue, magfilterValue, GCM_TEXTURE_CONVOLUTION_QUINCUNX);
	const int wrap = GetWrapModeEnum(GetWrapMode());

	rsxTextureWrapMode(context, textureUnitIndex, wrap, wrap, wrap, 0, GCM_TEXTURE_ZFUNC_LESS, 0);
}

int TexturePS3::GetWrapModeEnum(WrapMode wrapMode) const
{
	int mode = 0;
	switch (wrapMode)
	{
	case WrapMode::ClampToEdge:
		mode = GCM_TEXTURE_CLAMP_TO_EDGE;
		break;
	case WrapMode::Repeat:
		mode = GCM_TEXTURE_REPEAT;
		break;
	}
	return mode;
}

void TexturePS3::Unload()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	ClearSpriteSelections();
	rsxFree(m_ps3buffer);

#if defined (DEBUG)
	size_t byteCount = 0;
	if (!isFloatFormat)
	{
		byteCount = GetWidth() * GetHeight() * 4;
	}
	else if (isFloatFormat)
	{
		byteCount = GetWidth() * GetHeight() * 4 * sizeof(float);
	}
	Performance::s_textureMemoryTracker->Deallocate(byteCount);
#endif
}

#endif