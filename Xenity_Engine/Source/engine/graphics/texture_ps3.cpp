// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine
#if defined(__PS3__)
#include "texture_ps3.h"

#include <malloc.h>
#include <string>

#include <rsx/rsx.h>

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
#include <engine/graphics/renderer/renderer_rsx.h>
#include <engine/graphics/shader_rsx.h>
#include <engine/graphics/graphics.h>

#include "renderer/renderer.h"

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

void TexturePS3::SetData(const unsigned char* texData)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);
	XASSERT(texData != nullptr, "[TexturePS3::SetTextureLevel] texData is nullptr");

	if (!isFloatFormat)
	{
		m_ps3buffer = (unsigned char*)rsxMemalign(128, (GetWidth() * GetHeight() * 4));
	}
	else if (isFloatFormat)
	{
		m_ps3buffer = (unsigned char*)rsxMemalign(128, (GetWidth() * GetHeight() * 4 * sizeof(float)));
	}

	if (!m_ps3buffer)
		return;

	unsigned char* upBuffer = m_ps3buffer;
	for (int i = 0; i < GetWidth() * GetHeight() * 4; i += 4)
	{
		upBuffer[i + 0] = texData[(i + 3)];
		upBuffer[i + 1] = texData[(i + 0)];
		upBuffer[i + 2] = texData[(i + 1)];
		upBuffer[i + 3] = texData[(i + 2)];
	}

	isValid = true;
}

void TexturePS3::Bind() const
{
	if (!m_ps3buffer)
	{
		return;
	}

	uint32_t offset;
	rsxAddressToOffset(m_ps3buffer, &offset);
	gcmContextData* context = RendererRSX::context;
	rsxInvalidateTextureCache(context, GCM_INVALIDATE_TEXTURE);

	uint32_t resolutionMultiplier = 1;
	if (isFloatFormat)
	{
		resolutionMultiplier = 4;
	}

	gcmTexture gcmTexture;
	if (!isFloatFormat)
	{
		gcmTexture.format = (GCM_TEXTURE_FORMAT_A8R8G8B8 | GCM_TEXTURE_FORMAT_LIN);
	}
	else
	{
		gcmTexture.format = (GCM_TEXTURE_FORMAT_W32_Z32_Y32_X32_FLOAT | GCM_TEXTURE_FORMAT_LIN | GCM_TEXTURE_FORMAT_UNRM);
	}
	gcmTexture.mipmap = 1;
	gcmTexture.dimension = GCM_TEXTURE_DIMS_2D;
	gcmTexture.cubemap = GCM_FALSE;
	gcmTexture.remap = ((GCM_TEXTURE_REMAP_TYPE_REMAP << GCM_TEXTURE_REMAP_TYPE_B_SHIFT) |
		(GCM_TEXTURE_REMAP_TYPE_REMAP << GCM_TEXTURE_REMAP_TYPE_G_SHIFT) |
		(GCM_TEXTURE_REMAP_TYPE_REMAP << GCM_TEXTURE_REMAP_TYPE_R_SHIFT) |
		(GCM_TEXTURE_REMAP_TYPE_REMAP << GCM_TEXTURE_REMAP_TYPE_A_SHIFT) |
		(GCM_TEXTURE_REMAP_COLOR_B << GCM_TEXTURE_REMAP_COLOR_B_SHIFT) |
		(GCM_TEXTURE_REMAP_COLOR_G << GCM_TEXTURE_REMAP_COLOR_G_SHIFT) |
		(GCM_TEXTURE_REMAP_COLOR_R << GCM_TEXTURE_REMAP_COLOR_R_SHIFT) |
		(GCM_TEXTURE_REMAP_COLOR_A << GCM_TEXTURE_REMAP_COLOR_A_SHIFT));
	gcmTexture.width = GetWidth() * resolutionMultiplier;
	gcmTexture.height = GetHeight() * resolutionMultiplier;
	gcmTexture.depth = 1;
	gcmTexture.location = GCM_LOCATION_RSX;
	gcmTexture.pitch = GetWidth() * 4 * resolutionMultiplier;
	gcmTexture.offset = offset;

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

	rsxLoadTexture(context, textureUnitIndex, &gcmTexture);
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
	case WrapMode::ClampToBorder:
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
	Performance::s_textureMemoryTracker->Deallocate(m_width * height * 4);
#endif
}

#endif