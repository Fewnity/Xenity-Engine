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

#include "renderer/renderer.h"

TexturePS3::TexturePS3()
{
}

TexturePS3::~TexturePS3()
{
	Debug::Print("TexturePS3::~TexturePS3()" + std::to_string(textureId), true);
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

	m_ps3buffer = (unsigned char*)rsxMemalign(128, (GetWidth() * GetHeight() * 4));
	if (!m_ps3buffer)
		return;

	unsigned char* upBuffer = m_ps3buffer;
	for (int i = 0; i < GetWidth() * GetHeight() * 4; i += 4)
	{
		upBuffer[i + 0] = m_buffer[(i + 3)];
		upBuffer[i + 1] = m_buffer[(i + 0)];
		upBuffer[i + 2] = m_buffer[(i + 1)];
		upBuffer[i + 3] = m_buffer[(i + 2)];
	}

	isValid = true;
}

#endif