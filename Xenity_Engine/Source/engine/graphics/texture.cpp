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
#include "texture_default.h"
#include "texture_psp.h"
#include "texture_ps3.h"

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
#if defined(__PSP__)
	std::shared_ptr<Texture> newTexture = std::make_shared<TexturePSP>();
#elif defined(__PS3__)
	std::shared_ptr<Texture> newTexture = std::make_shared<TexturePS3>();
#else
	std::shared_ptr<Texture> newTexture = std::make_shared<TextureDefault>();
#endif
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

#if defined (DEBUG)
		Performance::s_textureMemoryTracker->Allocate(m_width * height * 4);
#endif
	}
	else
	{
		Debug::PrintError("[Texture::LoadTexture] Failed to open texture file", true);
		m_fileStatus = FileStatus::FileStatus_Failed;
		return;
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