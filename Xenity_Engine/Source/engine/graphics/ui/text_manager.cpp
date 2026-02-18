// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2026 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "text_manager.h"

#if defined(__PSP__)
#include <pspkernel.h>
#endif

#include <engine/graphics/graphics.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/debug/debug.h>
#include <engine/tools/profiler_benchmark.h>
#include <engine/game_elements/transform.h>
#include <engine/tools/internal_math.h>
#include <engine/graphics/texture/texture.h>
#include <engine/graphics/camera.h>
#include <engine/asset_management/asset_manager.h>
#include "font.h"
#include <glm/gtx/quaternion.hpp>

/**
 * @brief Init text manager
 *
 */
void TextManager::Init()
{
}

static int DecodeUTF8(const char* s, uint32_t& outCodepoint)
{
	const unsigned char* bytes = (const unsigned char*)s;

	if (bytes[0] < 0x80) // 1 byte (ASCII)
	{
		outCodepoint = bytes[0];
		return 1;
	}
	else if ((bytes[0] & 0xE0) == 0xC0) // 2 bytes
	{
		outCodepoint =
			((bytes[0] & 0x1F) << 6) |
			(bytes[1] & 0x3F);
		return 2;
	}
	else if ((bytes[0] & 0xF0) == 0xE0) // 3 bytes
	{
		outCodepoint =
			((bytes[0] & 0x0F) << 12) |
			((bytes[1] & 0x3F) << 6) |
			(bytes[2] & 0x3F);
		return 3;
	}
	else if ((bytes[0] & 0xF8) == 0xF0) // 4 bytes
	{
		outCodepoint =
			((bytes[0] & 0x07) << 18) |
			((bytes[1] & 0x3F) << 12) |
			((bytes[2] & 0x3F) << 6) |
			(bytes[3] & 0x3F);
		return 4;
	}

	// Invalid
	outCodepoint = 0;
	return 0; // Should not be 0 but for now, we only support the first 256 codepoints, so it will be good
}

std::shared_ptr<MeshData> TextManager::CreateMesh(const std::string &text, TextInfo *textInfo, HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment, const Color &color, const std::shared_ptr<Font> &font, float scale)
{
	if (!font->GetFontAtlas())
		return nullptr;

	int textLenght = 0;

	// Count the number of characters to draw
	const char* str = text.c_str();
	while (*str)
	{
		uint32_t codepoint = 0;
		const uint32_t byteCount = DecodeUTF8(str, codepoint);
		str += byteCount;

		// for now, only the first 256 codepoints are supported
		if (byteCount != 1)
		{
			continue;
		}

		textLenght++;
	}

	if (textLenght == 0)
	{
		return nullptr;
	}

	// Set text start offset
	float totalY = 0;
	for (int i = 0; i < textInfo->lineCount; i++)
	{
		totalY += textInfo->maxLineHeight;
	}

	float x = 0;
	float y = 0;
	int line = 0;
	if (horizontalAlignment == HorizontalAlignment::Left)
		x = textInfo->linesInfo[line].lenght * scale;
	else if (horizontalAlignment == HorizontalAlignment::Center)
		x = textInfo->linesInfo[line].lenght * 0.5f * scale;

	y = textInfo->linesInfo[line].y1 * 0.25f * scale;
	y += -textInfo->maxLineHeight * scale;

	if (verticalAlignment == VerticalAlignment::Center)
	{
		y += totalY * 0.5f * scale;
	}
	else if (verticalAlignment == VerticalAlignment::Top)
	{
		y += totalY * scale;
	}

	// Create empty mesh
	const int charCountToDraw = textLenght - (textInfo->lineCount - 1);

	VertexDescriptor vertexDescriptorList;
	vertexDescriptorList.AddVertexElement(VertexElement::UV_32_BITS);
#if defined(__vita__)
	vertexDescriptorList.AddVertexElement(VertexElement::COLOR_4_FLOATS);
#endif
	vertexDescriptorList.AddVertexElement(VertexElement::POSITION_32_BITS);


	std::shared_ptr<MeshData> mesh = MeshData::CreateMeshData();
	mesh->CreateSubMesh(6 * charCountToDraw, 6 * charCountToDraw, vertexDescriptorList);
	mesh->unifiedColor = color;

	int drawnCharIndex = 0;

	str = text.c_str();
	while (*str)
	{
		uint32_t codepoint = 0;
		const uint32_t byteCount = DecodeUTF8(str, codepoint);

		// for now, only the first 256 codepoints are supported
		if (byteCount != 1)
		{
			str += byteCount;
			continue;
		}

		const char c = *str;
		Character *ch = font->Characters[(int)c];
		if (c == '\n')
		{
			line++;

			if (horizontalAlignment == HorizontalAlignment::Left)
				x = textInfo->linesInfo[line].lenght;
			else if (horizontalAlignment == HorizontalAlignment::Center)
				x = textInfo->linesInfo[line].lenght * 0.5f;
			else
				x = 0;

			y += -textInfo->maxLineHeight * scale;
		}
		else
		{
			AddCharToMesh(mesh, ch, x, y, drawnCharIndex, scale);
			drawnCharIndex++;
			x -= ch->rightAdvance * scale;
		}
		str += byteCount;
	}

	mesh->OnLoadFileReferenceFinished();

#if defined(__PSP__)
	sceKernelDcacheWritebackInvalidateAll(); // Very important
#endif
	return mesh;
}

void TextManager::DrawText(const std::string &text, TextInfo *textInfo, HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment, const Transform &transform, const Color& color, bool canvas, const MeshData& mesh, const Font& font, Material& material)
{
	if (!font.GetFontAtlas() || !font.GetFontAtlas()->IsValid())
	{
		Debug::PrintError("[TextManager::DrawText] Invalid font", true);
		return;
	}

	if (Graphics::usedCamera)
	{
		RenderingSettings renderSettings = RenderingSettings();
		if (transform.GetScale().x * transform.GetScale().y < 0)
			renderSettings.invertFaces = true;
		else
			renderSettings.invertFaces = false;

		renderSettings.renderingMode = MaterialRenderingMode::Transparent;
		renderSettings.useDepth = !canvas;
		renderSettings.useTexture = true;
		renderSettings.useLighting = !canvas;

		const glm::mat4& transformationMatrix = transform.GetTransformationMatrix();

		if (Graphics::usedCamera->IsEditor() || !canvas)
		{
			const glm::mat4 MVP = Graphics::usedCamera->m_viewProjectionMatrix * transformationMatrix;

			Graphics::DrawSubMesh(*mesh.m_subMeshes[0], material, font.GetFontAtlas().get(), renderSettings, transformationMatrix, transformationMatrix, MVP, canvas);
		}
		else 
		{
			static const glm::mat4 canvasCameraViewMatrix = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
			const glm::mat4 MVP = (Graphics::usedCamera->m_canvasProjection * canvasCameraViewMatrix) * transformationMatrix;

			Graphics::DrawSubMesh(*mesh.m_subMeshes[0], material, font.GetFontAtlas().get(), renderSettings, transformationMatrix, transformationMatrix, MVP, canvas);
		}
	}
}

void TextManager::AddCharToMesh(const std::shared_ptr<MeshData> &mesh, Character *ch, float x, float y, int letterIndex, float scale)
{
	const int indice = letterIndex * 6;
	const int indiceIndex = letterIndex * 6;

	const float w = -ch->rightSize.x * scale;
	const float h = ch->rightSize.y * scale;

	const float fixedY = (y - (ch->rightSize.y - ch->rightBearing.y) * scale);

	// Use 6 vertices instead of 4 because at the time the PS2 VU1 renderer do not supports indices

	std::unique_ptr<MeshData::SubMesh>& subMesh = mesh->m_subMeshes[0];

	const Color white = Color::CreateFromRGBFloat(1, 1, 1);
#if defined(__vita__)
	subMesh->SetVertex(ch->uv.x, ch->uv.y, white, w + x, fixedY, 0, indice);
	subMesh->SetVertex(ch->uvOffet.x, ch->uv.y, white, x, fixedY, 0, 1 + indice);
	subMesh->SetVertex(ch->uvOffet.x, ch->uvOffet.y, white, x, h + fixedY, 0, 2 + indice);

	subMesh->SetVertex(ch->uv.x, ch->uv.y, white, w + x, fixedY, 0, 3 + indice);
	subMesh->SetVertex(ch->uv.x, ch->uvOffet.y, white, w + x, h + fixedY, 0, 4 + indice);
	subMesh->SetVertex(ch->uvOffet.x, ch->uvOffet.y, white, x, h + fixedY, 0, 5 + indice);
#else
	subMesh->SetVertex(ch->uv.x, ch->uv.y, w + x, fixedY, 0, indice);
	subMesh->SetVertex(ch->uvOffet.x, ch->uv.y, x, fixedY, 0, 1 + indice);
	subMesh->SetVertex(ch->uvOffet.x, ch->uvOffet.y, x, h + fixedY, 0, 2 + indice);

	subMesh->SetVertex(ch->uv.x, ch->uv.y, w + x, fixedY, 0, 3 + indice);
	subMesh->SetVertex(ch->uv.x, ch->uvOffet.y, w + x, h + fixedY, 0, 4 + indice);
	subMesh->SetVertex(ch->uvOffet.x, ch->uvOffet.y, x, h + fixedY, 0, 5 + indice);
#endif
	subMesh->SetIndex(indiceIndex + 0, indice + 0);
	subMesh->SetIndex(indiceIndex + 1, indice + 2);
	subMesh->SetIndex(indiceIndex + 2, indice + 1);
	subMesh->SetIndex(indiceIndex + 3, indice + 3);
	subMesh->SetIndex(indiceIndex + 4, indice + 4);
	subMesh->SetIndex(indiceIndex + 5, indice + 5);
}

TextInfo *TextManager::GetTextInformations(const std::string &text, std::shared_ptr<Font> font, float scale)
{
	TextInfo *textInfos = new TextInfo();
	if (!font || !font->GetFontAtlas())
		return textInfos;

	textInfos->linesInfo.emplace_back(LineInfo());

	int currentLine = 0;
	float higherY = 0;
	float lowerY = 0;

	const char* str = text.c_str();
	while (*str)
	{
		uint32_t codepoint = 0;
		const uint32_t byteCount = DecodeUTF8(str, codepoint);

		// for now, only the first 256 codepoints are supported
		if (byteCount != 1)
		{
			str += byteCount;
			continue;
		}

		const Character *ch = font->Characters[codepoint];
		if (*str == '\n')
		{
			textInfos->linesInfo[currentLine].lenght *= scale;
			textInfos->linesInfo[currentLine].y1 = (higherY - lowerY) * scale;
			textInfos->linesInfo.emplace_back(LineInfo());
			currentLine++;
			higherY = 0;
			lowerY = 0;
		}
		else
		{
			textInfos->linesInfo[currentLine].lenght += ch->rightAdvance;
			if (higherY < ch->rightBearing.y)
				higherY = ch->rightBearing.y;

			float low = ch->rightSize.y - ch->rightBearing.y;
			if (lowerY < low)
				lowerY = low;
		}
		str += byteCount;
	}
	textInfos->linesInfo[currentLine].lenght *= scale;
	textInfos->linesInfo[currentLine].y1 = (higherY - lowerY) * scale;

	textInfos->maxLineHeight = font->maxCharHeight * scale;
	textInfos->lineCount = currentLine + 1;

	return textInfos;
}
