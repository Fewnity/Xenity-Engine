// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "texture_default.h"

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
#include <glad/glad.h>
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

TextureDefault::TextureDefault()
{
}

TextureDefault::~TextureDefault()
{
	Debug::Print("TextureDefault::~TextureDefault()" + std::to_string(textureId), true);
	this->UnloadFileReference();
}

void TextureDefault::OnLoadFileReferenceFinished()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	textureId = Engine::GetRenderer().CreateNewTexture();
	Engine::GetRenderer().BindTexture(*this);
	const unsigned int rgba = 0x1908;
	Engine::GetRenderer().SetTextureData(*this, rgba, m_buffer);

	free(m_buffer);
	isValid = true;
}

// TODO: This function only supports 1 color textures
void TextureDefault::SetData(const unsigned char *texData)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	XASSERT(texData != nullptr, "[TextureDefault::SetTextureLevel] texData is nullptr");

	textureId = Engine::GetRenderer().CreateNewTexture();
	Engine::GetRenderer().BindTexture(*this);
	// unsigned int alpha = 0x1906;

	Engine::GetRenderer().SetTextureData(*this, GL_LUMINANCE_ALPHA, texData);

	isValid = true;
}