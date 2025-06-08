// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
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
#include <glad/gl.h>
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
#include <engine/graphics/renderer/renderer.h>

#include "texture_default.h"
#include "texture_psp.h"
#include "texture_ps3.h"

Texture::Texture()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	std::unique_ptr<TextureSettings> textureSettingsStandalone = std::make_unique<TextureSettingsStandalone>();
	std::unique_ptr<TextureSettings> textureSettingsPSP = std::make_unique<TextureSettingsPSP>();
	std::unique_ptr<TextureSettings> textureSettingsPSVITA = std::make_unique<TextureSettingsPSVITA>();
	std::unique_ptr<TextureSettings> textureSettingsPS3 = std::make_unique<TextureSettingsPS3>();

	m_settings[AssetPlatform::AP_Standalone] = std::move(textureSettingsStandalone);
	m_settings[AssetPlatform::AP_PSP] = std::move(textureSettingsPSP);
	m_settings[AssetPlatform::AP_PsVita] = std::move(textureSettingsPSVITA);
	m_settings[AssetPlatform::AP_PS3] = std::move(textureSettingsPS3);
}

std::shared_ptr<Texture> Texture::CreateTexture(const TextureConstructorParams& params)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);
	std::shared_ptr<Texture> newTexture = MakeTexture(true);

	newTexture->SetSize(params.width, params.width);
	newTexture->SetChannelCount(params.hasAlpha ? 4 : 3);
#if defined(__PSP__)
	reinterpret_cast<TextureSettingsPSP*>(newTexture->m_settings[AssetPlatform::AP_PSP].get())->type = params.pspTextureType;
#elif defined(__PS3__)
	reinterpret_cast<TextureSettingsPS3*>(newTexture->m_settings[AssetPlatform::AP_PS3].get())->type = params.ps3TextureType;
#endif
	newTexture->SetFilter(params.filter);
	newTexture->SetWrapMode(params.wrapMode);

	return newTexture;
}

ReflectiveData Texture::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

ReflectiveData Texture::GetMetaReflectiveData(AssetPlatform platform)
{
	ReflectiveData reflectedVariables;
	// Add platform specific settings variables to the list of reflected variables
	ReflectiveData reflectedVariablesPlatform = m_settings[platform]->GetReflectiveData();
	reflectedVariables.insert(reflectedVariables.end(), reflectedVariablesPlatform.begin(), reflectedVariablesPlatform.end());
	return reflectedVariables;
}

void Texture::OnReflectionUpdated()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

#if defined(EDITOR)
	if (previousResolution != GetCookResolution() && m_fileStatus == FileStatus::FileStatus_Loaded && isValid)
	{
		previousResolution = GetCookResolution();
		UnloadFileReference();

		FileReference::LoadOptions loadOptions;
		loadOptions.platform = Application::GetPlatform();
		loadOptions.threaded = false;
		LoadFileReference(loadOptions);
	}
#endif
}

std::shared_ptr<Texture> Texture::MakeTexture(bool isDynamic)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

#if defined(__PSP__)
	std::shared_ptr<Texture> newTexture = std::make_shared<TexturePSP>();
#elif defined(__PS3__)
	std::shared_ptr<Texture> newTexture = std::make_shared<TexturePS3>();
#else
	std::shared_ptr<Texture> newTexture = std::make_shared<TextureDefault>();
#endif
	if (!isDynamic)
	{
		AssetManager::AddFileReference(newTexture);
	}

	return newTexture;
}

void Texture::LoadFileReference(const LoadOptions& loadOptions)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	if (m_fileStatus == FileStatus::FileStatus_Not_Loaded)
	{
		m_fileStatus = FileStatus::FileStatus_Loading;

#if defined(EDITOR)
		if (!loadOptions.threaded)
		{
			LoadTexture();
			OnLoadFileReferenceFinished();
		}
		else
		{
			AsyncFileLoading::AddFile(shared_from_this());
			std::thread threadLoading = std::thread(&Texture::LoadTexture, this);
			threadLoading.detach();
		}
#else
		LoadTexture();
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

void Texture::LoadTexture()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	size_t fileBufferSize = 0;
	unsigned char* fileData = ReadBinary(fileBufferSize);
	if (!fileData)
	{
		Debug::PrintError("[Texture::LoadTexture] Failed to open texture file", true);
		m_fileStatus = FileStatus::FileStatus_Failed;
		return;
	}

	// Only for editor, live resizing
#if defined(EDITOR)
		// Load image with stb_image
	unsigned char* data2 = stbi_load_from_memory(fileData, static_cast<int>(fileBufferSize), &m_width, &height,
		&nrChannels, 4);
	free(fileData);

	m_originalWidth = m_width;
	m_originalHeight = height;

	int newWidth = m_width;
	int newHeight = height;
	const int cookResolution = static_cast<int>(GetCookResolution());
	if ((newWidth > height) && newWidth > cookResolution)
	{
		newHeight = static_cast<int>(height * (static_cast<float>(GetCookResolution()) / static_cast<float>(m_width)));
		newWidth = cookResolution;
	}
	else if ((newHeight > m_width) && newHeight > cookResolution)
	{
		newWidth = static_cast<int>(m_width * (static_cast<float>(GetCookResolution()) / static_cast<float>(height)));
		newHeight = cookResolution;
	}
	else if ((newWidth == newHeight) && newWidth > cookResolution)
	{
		newWidth = cookResolution;
		newHeight = cookResolution;
	}

	if (newWidth == m_width && newHeight == height)
	{
		m_buffer = data2;
	}
	else
	{
		// Resize image
		m_buffer = static_cast<unsigned char*>(malloc(newWidth * newHeight * 4));
		stbir_resize_uint8(data2, m_width, height, 0, m_buffer, newWidth, newHeight, 0, 4);
		free(data2);
	}
	m_width = newWidth;
	height = newHeight;
#else
		// Load image with stb_image
	m_buffer = stbi_load_from_memory(fileData, static_cast<int>(fileBufferSize), &m_width, &height,
		&nrChannels, 4);

	free(fileData);
#endif

	if (!m_buffer)
	{
		Debug::PrintError("[Texture::LoadTexture] Failed to load texture: " + m_file->GetPath(), true);
		m_fileStatus = FileStatus::FileStatus_Failed;
		return;
	}

	m_fileStatus = FileStatus::FileStatus_AsyncWaiting;
}

#pragma endregion