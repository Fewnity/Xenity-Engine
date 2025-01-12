// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "font.h"

#if !defined(__PS3__) // Not support on PS3 currently, need to build ft2
#include <ft2build.h>
#include FT_FREETYPE_H
#endif

#if defined(__PSP__)
#include <pspkernel.h>
#elif defined(__vita__)
#include <vitaGL.h>
#endif

#include <engine/graphics/texture.h>
#include <engine/debug/debug.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/file_system/file.h>
#include <engine/debug/stack_debug_object.h>

Font::~Font()
{
}

std::shared_ptr<Font> Font::MakeFont()
{
	std::shared_ptr<Font> newFileRef = std::make_shared<Font>();
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
}

ReflectiveData Font::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

ReflectiveData Font::GetMetaReflectiveData([[maybe_unused]] AssetPlatform platform)
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

void Font::OnReflectionUpdated()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);
}

void Font::LoadFileReference(const LoadOptions& loadOptions)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	if (m_fileStatus == FileStatus::FileStatus_Not_Loaded)
	{
		bool result = CreateFont(*this);
		if (result) 
		{
			m_fileStatus = FileStatus::FileStatus_Loaded;
		}
		else
		{
			m_fileStatus = FileStatus::FileStatus_Failed;
		}
	}
}

bool Font::CreateFont(Font& font)
{
	Debug::Print("Loading font: " + font.m_file->GetPath(), true);
#if !defined(__LINUX__) && !defined(__PS3__)
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		Debug::PrintError("[Font::CreateFont] Could not init FreeType Library", true);
		return false;
	}

	// Load font
	FT_Face face;
#if defined(EDITOR)
	if (FT_New_Face(ft, font.m_file->GetPath().c_str(), 0, &face))
	{
		Debug::PrintError("[Font::CreateFont] Failed to load font", true);
		return false;
	}
#else
	const int fileBufferSize = m_fileSize;
	unsigned char* fileData = nullptr;
	fileData = ProjectManager::fileDataBase.GetBitFile().ReadBinary(m_filePosition, fileBufferSize);
	if (FT_New_Memory_Face(ft, fileData, fileBufferSize, 0, &face))
	{
		Debug::PrintError("[Font::CreateFont] Failed to load font from memory", true);
		return false;
	}
#endif
	int charPixelHeight = 48;
	// int charPixelHeight = 21;
	//  Load glyph
	FT_Set_Pixel_Sizes(face, 0, charPixelHeight);
	// Engine::renderer->PixelStoreUnpack();
	// glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// int atlasSize = 256;
	int atlasSize = 512;
	//  int atlasSize = 256;
	int channelCount = 2;
#if defined(__PSP__) || defined(_EE)
	channelCount = 4;
#endif

	unsigned char *atlas = (unsigned char *)calloc((size_t)atlasSize * atlasSize * channelCount, sizeof(unsigned char));
	if (!atlas)
	{
		return false;
	}

	int xOffset = 0;
	int yOffset = 0;
	for (unsigned char c = 0; c < 255; c++)
	{
		try
		{
			// load character glyph
			if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0)
			{
				Debug::PrintError("[Font::CreateFont] Failed to load Glyph. Path: " + font.m_file->GetPath(), true);
				continue;
			}

			// now store character for later use
			Character *character = new Character();
			character->Size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
			character->Bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
			character->rightSize = Vector2(face->glyph->bitmap.width * 0.01f, face->glyph->bitmap.rows * 0.01f);
			character->rightBearing = Vector2(face->glyph->bitmap_left * 0.01f, face->glyph->bitmap_top * 0.01f);
			character->Advance = (unsigned int)face->glyph->advance.x;
			character->rightAdvance = (face->glyph->advance.x >> 6) * 0.01f;

			font.Characters[c] = character;

			if (font.maxCharHeight < (float)character->rightSize.y)
				font.maxCharHeight = (float)character->rightSize.y;

			if (int(xOffset + face->glyph->bitmap.width) >= atlasSize)
			{
				xOffset = 0;
				yOffset += charPixelHeight;
			}

			character->uvOffet = Vector2(xOffset / (float)atlasSize, yOffset / (float)atlasSize);
			character->uv = Vector2((xOffset + face->glyph->bitmap.width) / (float)atlasSize, (yOffset + face->glyph->bitmap.rows) / (float)atlasSize);

			if (c >= 32) // Do not render invisible chars
			{
				int textureXOffset = xOffset * channelCount;
				for (int fW = 0; fW < (int)face->glyph->bitmap.rows; fW++)
				{
					for (int fH = 0; fH < (int)face->glyph->bitmap.width; fH++)
					{
						int atlasOffset = (fH * channelCount) + (fW * atlasSize * channelCount) + textureXOffset + yOffset * atlasSize * channelCount;
#if defined(__PSP__)
						atlas[atlasOffset] = 255;
						atlas[atlasOffset + 1] = 255;
						atlas[atlasOffset + 2] = 255;
						atlas[atlasOffset + 3] = face->glyph->bitmap.buffer[fH + (fW * face->glyph->bitmap.width)];
#elif defined(_EE)
						atlas[atlasOffset] = face->glyph->bitmap.buffer[fH + (fW * face->glyph->bitmap.width)];
						atlas[atlasOffset + 1] = face->glyph->bitmap.buffer[fH + (fW * face->glyph->bitmap.width)];
						atlas[atlasOffset + 2] = face->glyph->bitmap.buffer[fH + (fW * face->glyph->bitmap.width)];
						atlas[atlasOffset + 3] = 255;
#else
						// atlas[atlasOffset] = face->glyph->bitmap.buffer[fH + (fW * face->glyph->bitmap.width)];
						atlas[atlasOffset] = 255;
						atlas[atlasOffset + 1] = face->glyph->bitmap.buffer[fH + (fW * face->glyph->bitmap.width)];
#endif
					}
				}
				xOffset += face->glyph->bitmap.width + 1;
			}
		}
		catch (...)
		{
			Debug::PrintError("[Font::CreateFont] Failed to load Glyph. Path: " + font.m_file->GetPath(), true);
			free(atlas);
			return false;
		}
	}

	std::shared_ptr<Texture> newAtlas = Texture::MakeTexture();
	newAtlas->SetSize(atlasSize, atlasSize);
	newAtlas->SetChannelCount(channelCount);
#if defined(__PSP__)
	reinterpret_cast<TextureSettingsPSP*>(newAtlas->m_settings[AssetPlatform::AP_PSP].get())->type = PSPTextureType::RGBA_4444;
#endif
	newAtlas->SetData(atlas);
	newAtlas->SetFilter(Filter::Bilinear);
	newAtlas->SetWrapMode(WrapMode::ClampToEdge);

	font.fontAtlas = newAtlas;

	free(atlas);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
#if !defined(EDITOR)
	free(fileData);
#endif
#if defined(__PSP__)
	sceKernelDcacheWritebackInvalidateAll(); // Very important
#endif

	Debug::Print("Font loaded", true);
#endif
	return true;
}