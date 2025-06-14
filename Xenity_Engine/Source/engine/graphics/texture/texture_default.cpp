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
#include <engine/tools/internal_math.h>
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
	XASSERT(Engine::IsCalledFromMainThread(), "Function called from another thread");

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
		mode = GL_CLAMP_TO_EDGE;
//#if defined(_WIN32) || defined(_WIN64) || defined (__LINUX__)
//#else
//		mode = GL_CLAMP;
//#endif
		break;
	case WrapMode::Repeat:
		mode = GL_REPEAT;
		break;
	}
	return mode;
}

void TextureDefault::ApplyTextureFilters() const
{
	XASSERT(Engine::IsCalledFromMainThread(), "Function called from another thread");

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

	SetData(m_buffer);

	free(m_buffer);
}

// TODO: This function only supports 1 color textures, add enum for texture color type
void TextureDefault::SetData(const unsigned char *texData)
{
	XASSERT(Engine::IsCalledFromMainThread(), "Function called from another thread");

	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	XASSERT(texData != nullptr, "[TextureDefault::SetTextureLevel] texData is nullptr");

	if (m_textureId == -1)
	{
		glGenTextures(1, &m_textureId);
#if defined (DEBUG)
		Performance::s_textureMemoryTracker->Allocate(m_width * height * 4);
#endif
	}
	Bind();

	//const unsigned int textureType = GL_LUMINANCE_ALPHA;
	const unsigned int textureType = GL_RGBA; // rgba
	glTexImage2D(GL_TEXTURE_2D, 0, textureType, GetWidth(), GetHeight(), 0, textureType, GL_UNSIGNED_BYTE, texData);
	if (GetUseMipmap())
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	isValid = true;
	m_fileStatus = FileStatus::FileStatus_Loaded;
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