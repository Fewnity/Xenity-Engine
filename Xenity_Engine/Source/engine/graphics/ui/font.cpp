// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2026 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "font.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#if defined(__PSP__)
#include <pspkernel.h>
#elif defined(__vita__)
#include <vitaGL.h>
#endif

#include <string>

#include <engine/graphics/texture/texture.h>
#include <engine/debug/debug.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/file_system/file.h>
#include <engine/debug/stack_debug_object.h>
#include <engine/engine.h>
#include <engine/file_system/async_file_loading.h>

Font::~Font()
{
	if (m_atlasBuffer)
	{
		delete[] m_atlasBuffer;
		m_atlasBuffer = nullptr;
	}
	for (int i = 0; i < 256; i++)
	{
		delete Characters[i];
		Characters[i] = nullptr;
	}
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
		m_fileStatus = FileStatus::FileStatus_Loading;

		if (!Engine::IsCalledFromMainThread() && !loadOptions.forceDisableAsync)
		{
			AsyncFileLoading::AddFile(shared_from_this());
		}

		const bool result = CreateFont(*this);
		if (result) 
		{
			if (Engine::IsCalledFromMainThread() || loadOptions.forceDisableAsync)
			{
				OnLoadFileReferenceFinished();
			}
			else
			{
				m_fileStatus = FileStatus::FileStatus_AsyncWaiting;
			}
		}
		else
		{
			m_fileStatus = FileStatus::FileStatus_Failed;
		}
	}
}

void Font::OnLoadFileReferenceFinished()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	// Called by the async loading even if the loading has failed, keep the failed status
	if (m_fileStatus == FileStatus::FileStatus_Failed)
	{
		return;
	}

	if (fontAtlas && m_atlasBuffer)
	{
		fontAtlas->SetData(m_atlasBuffer);
		delete[] m_atlasBuffer;
		m_atlasBuffer = nullptr;
	}
	m_fileStatus = FileStatus::FileStatus_Loaded;
}

#if !defined(__LINUX__)
/**
 * @brief Check if all printable glyphs fit inside an atlas of given size and pixel height
 */
static bool CheckFontPacking(FT_Face face, int pixelHeight, int atlasSize)
{
	if (FT_Set_Pixel_Sizes(face, 0, pixelHeight) != 0)
	{
		return false;
	}

	int xOffset = 0;
	int yOffset = 0;
	int currentLineHeight = 0;

	for (unsigned char c = 32; c < 255; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0)
		{
			continue;
		}

		const int glyphWidth = face->glyph->bitmap.width;
		const int glyphRows = face->glyph->bitmap.rows;

		// Wrap to next line if glyph exceeds line width
		if (xOffset + glyphWidth >= atlasSize)
		{
			xOffset = 0;
			yOffset += currentLineHeight + 1;
			currentLineHeight = 0;
		}

		if (glyphRows > currentLineHeight)
		{
			currentLineHeight = glyphRows;
		}

		// Check if glyph exceeds atlas height
		if (yOffset + glyphRows >= atlasSize)
		{
			return false;
		}

		xOffset += glyphWidth + 1;
	}

	return true;
}
#endif

#if !defined(__LINUX__)
namespace
{
	// Free the FreeType objects and the font file data on every return path
	// The file data is used by the face, so it's deleted after the face
	struct FreeTypeCleanup
	{
		FT_Library library = nullptr;
		FT_Face face = nullptr;
		unsigned char* fileData = nullptr;

		~FreeTypeCleanup()
		{
			if (face)
				FT_Done_Face(face);
			if (library)
				FT_Done_FreeType(library);
			delete[] fileData;
		}
	};
}
#endif

bool Font::CreateFont(Font& font)
{
	Debug::Print("Loading font: " + font.m_file->GetPath(), true);
#if !defined(__LINUX__)
	FreeTypeCleanup cleanup;
	if (FT_Init_FreeType(&cleanup.library))
	{
		cleanup.library = nullptr;
		Debug::PrintError("[Font::CreateFont] Could not init FreeType Library", true);
		return false;
	}
	FT_Library ft = cleanup.library;

	// Load font
	FT_Face face = nullptr;
#if defined(EDITOR)
	if (FT_New_Face(ft, font.m_file->GetPath().c_str(), 0, &face))
	{
		Debug::PrintError("[Font::CreateFont] Failed to load font", true);
		return false;
	}
#else
	const size_t fileBufferSize = m_fileSize;
	cleanup.fileData = ProjectManager::s_fileDataBase.GetBitFile().ReadBinary(m_filePosition, fileBufferSize);
	if (!cleanup.fileData || FT_New_Memory_Face(ft, cleanup.fileData, static_cast<FT_Long>(fileBufferSize), 0, &face))
	{
		Debug::PrintError("[Font::CreateFont] Failed to load font from memory", true);
		return false;
	}
#endif
	cleanup.face = face;
	const int atlasSize = 512;
	const int targetPixelHeight = 48;
	int charPixelHeight = targetPixelHeight;

	// Search for the maximum pixel size that fits in the atlas
	if (!CheckFontPacking(face, charPixelHeight, atlasSize))
	{
		int low = 8;
		int high = targetPixelHeight - 1;
		int bestSize = 8;

		while (low <= high)
		{
			const int mid = low + (high - low) / 2;
			if (CheckFontPacking(face, mid, atlasSize))
			{
				bestSize = mid;
				low = mid + 1; // Try larger
			}
			else
			{
				high = mid - 1; // Try smaller
			}
		}
		charPixelHeight = bestSize;
	}

	// Normalization factor so the text rendered in-game maintains its intended size
	const float scaleFactor = static_cast<float>(targetPixelHeight) / static_cast<float>(charPixelHeight);

	FT_Set_Pixel_Sizes(face, 0, charPixelHeight);

	int channelCount = 4;
#if defined(__PSP__) || defined(_EE) || defined(__PS3__)
	channelCount = 4;
#endif

	unsigned char *atlas = new unsigned char[atlasSize * atlasSize * channelCount];
	if (!atlas)
	{
		return false;
	}
	memset(atlas, 0, atlasSize * atlasSize * channelCount);
	int xOffset = 0;
	int yOffset = 0;
	int currentLineHeight = 0;
	for (unsigned char c = 0; c < 255; c++)
	{
		try
		{
			// Load character glyph
			if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0)
			{
				Debug::PrintError("[Font::CreateFont] Failed to load Glyph. Path: " + font.m_file->GetPath(), true);
				continue;
			}

			// Store character for later use
			Character *character = new Character();
			character->Size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
			character->Bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
			character->rightSize = Vector2(face->glyph->bitmap.width * 0.01f * scaleFactor, face->glyph->bitmap.rows * 0.01f * scaleFactor);
			character->rightBearing = Vector2(face->glyph->bitmap_left * 0.01f * scaleFactor, face->glyph->bitmap_top * 0.01f * scaleFactor);
			character->Advance = (unsigned int)face->glyph->advance.x;
			character->rightAdvance = (face->glyph->advance.x >> 6) * 0.01f * scaleFactor;

			font.Characters[c] = character;

			if (font.maxCharHeight < (float)character->rightSize.y)
				font.maxCharHeight = (float)character->rightSize.y;

			if (c >= 32) // Do not render invisible chars
			{
				if (int(xOffset + face->glyph->bitmap.width) >= atlasSize)
				{
					xOffset = 0;
					yOffset += currentLineHeight + 1;
					currentLineHeight = 0;
				}

				if (face->glyph->bitmap.rows > currentLineHeight)
				{
					currentLineHeight = face->glyph->bitmap.rows;
				}

				character->uvOffet = Vector2(xOffset / (float)atlasSize, yOffset / (float)atlasSize);
				character->uv = Vector2((xOffset + face->glyph->bitmap.width) / (float)atlasSize, (yOffset + face->glyph->bitmap.rows) / (float)atlasSize);

				const int textureXOffset = xOffset * channelCount;
				for (int fW = 0; fW < (int)face->glyph->bitmap.rows; fW++)
				{
					for (int fH = 0; fH < (int)face->glyph->bitmap.width; fH++)
					{
						const int atlasOffset = (fH * channelCount) + (fW * atlasSize * channelCount) + textureXOffset + yOffset * atlasSize * channelCount;
						if (atlasOffset + 3 < atlasSize * atlasSize * channelCount)
						{
#if defined(__PSP__) || defined(__PS3__)
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
							atlas[atlasOffset] = 255;
							atlas[atlasOffset + 1] = 255;
							atlas[atlasOffset + 2] = 255;
							atlas[atlasOffset + 3] = face->glyph->bitmap.buffer[fH + (fW * face->glyph->bitmap.width)];
							/*atlas[atlasOffset] = 255;
							atlas[atlasOffset + 1] = face->glyph->bitmap.buffer[fH + (fW * face->glyph->bitmap.width)];*/
#endif
						}
					}
				}
				xOffset += face->glyph->bitmap.width + 1;
			}
		}
		catch (...)
		{
			Debug::PrintError("[Font::CreateFont] Failed to load Glyph. Path: " + font.m_file->GetPath(), true);
			delete[] atlas;
			font.m_atlasBuffer = nullptr;
			return false;
		}
	}

	TextureConstructorParams params;
	params.width = atlasSize;
	params.height = atlasSize;
	params.filter = Filter::Bilinear;
	params.wrapMode = WrapMode::ClampToEdge;
	params.pspTextureType = PSPTextureType::RGBA_4444;
	params.ps3TextureType = PS3TextureType::ARGB_4444;

	std::shared_ptr<Texture> newAtlas = Texture::CreateTexture(params);

	font.fontAtlas = newAtlas;
	font.m_atlasBuffer = atlas;

#if defined(__PSP__)
	sceKernelDcacheWritebackInvalidateAll(); // Very important
#endif

	Debug::Print("Font loaded", true);
#endif
	return true;
}