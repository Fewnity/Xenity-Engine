// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "texture_default.h"

#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__) || defined(__vita__)

#include <malloc.h>
#include <string>
//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
//#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>

#if defined(__vita__)
#include <vitaGL.h>
#endif

#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
#include <thread>
#include <glad/gl.h>
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
#include <engine/graphics/renderer/renderer.h>


TextureDefault::~TextureDefault()
{
	Debug::Print("TextureDefault::~TextureDefault()" + std::to_string(m_textureId), true);
	this->UnloadFileReference();
}

void TextureDefault::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_textureId);
	ApplyTextureFilters();
	//float borderColor[] = { 1.0f, 1.0f, 1.0f, 0.0f };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
}

int TextureDefault::GetWrapModeEnum(WrapMode wrapMode) const
{
	int mode = GL_REPEAT;
	switch (wrapMode)
	{
	case WrapMode::ClampToEdge:
	case WrapMode::ClampToBorder:
		mode = GL_CLAMP_TO_EDGE;
//#if defined(_WIN32) || defined(_WIN64) || defined (__LINUX__)
//#else
//		mode = GL_CLAMP;
//#endif
		break;
	case WrapMode::Repeat:
		mode = GL_REPEAT;
		break;

		// case WrapMode::ClampToEdge:
		// 	mode = GL_CLAMP_TO_EDGE;
		// 	break;
		// case WrapMode::ClampToBorder:
		// 	mode = GL_CLAMP_TO_BORDER;
		// 	break;
		// case WrapMode::MirroredRepeat:
		// 	mode = GL_MIRRORED_REPEAT;
		// 	break;
		// case WrapMode::Repeat:
		// 	mode = GL_REPEAT;
		// 	break;
		// case WrapMode::MirrorClampToEdge:
		// 	mode = GL_MIRROR_CLAMP_TO_EDGE;
		// 	break;
	}
	return mode;
}

void TextureDefault::ApplyTextureFilters() const
{
	// Get the right filter depending of the texture settings
	int minFilterValue = GL_LINEAR;
	int magfilterValue = GL_LINEAR;
	if (GetFilter() == Filter::Bilinear)
	{
		if (GetUseMipmap())
		{
			minFilterValue = GL_LINEAR_MIPMAP_LINEAR;
		}
		else
		{
			minFilterValue = GL_LINEAR;
		}
		magfilterValue = GL_LINEAR;
	}
	else if (GetFilter() == Filter::Point)
	{
		if (GetUseMipmap())
		{
			minFilterValue = GL_NEAREST_MIPMAP_NEAREST;
		}
		else
		{
			minFilterValue = GL_NEAREST;
		}
		magfilterValue = GL_NEAREST;
	}

	const int wrap = GetWrapModeEnum(GetWrapMode());
	
	// Apply filters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilterValue);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magfilterValue);
}

void TextureDefault::OnLoadFileReferenceFinished()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	glGenTextures(1, &m_textureId);
	Bind();

	const unsigned int textureType = GL_RGBA; // rgba
	glTexImage2D(GL_TEXTURE_2D, 0, textureType, GetWidth(), GetHeight(), 0, textureType, GL_UNSIGNED_BYTE, m_buffer);
	if (GetUseMipmap())
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	free(m_buffer);
	isValid = true;
}

// TODO: This function only supports 1 color textures, add enum for texture color type
void TextureDefault::SetData(const unsigned char *texData)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	XASSERT(texData != nullptr, "[TextureDefault::SetTextureLevel] texData is nullptr");

	glGenTextures(1, &m_textureId);
	Bind();

	const unsigned int textureType = GL_LUMINANCE_ALPHA;
	glTexImage2D(GL_TEXTURE_2D, 0, textureType, GetWidth(), GetHeight(), 0, textureType, GL_UNSIGNED_BYTE, texData);
	if (GetUseMipmap())
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	isValid = true;
}

void TextureDefault::Unload()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	ClearSpriteSelections();
	if (m_textureId != -1)
	{
		glDeleteTextures(1, &m_textureId);
		m_textureId = -1;
#if defined (DEBUG)
		Performance::s_textureMemoryTracker->Deallocate(m_width * height * 4);
#endif
	}
}

#endif