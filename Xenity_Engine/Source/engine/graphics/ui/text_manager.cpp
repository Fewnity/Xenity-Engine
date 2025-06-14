// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
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

std::shared_ptr<MeshData> TextManager::CreateMesh(const std::string &text, TextInfo *textInfo, HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment, const Color &color, const std::shared_ptr<Font> &font, float scale)
{
	if (!font->GetFontAtlas())
		return nullptr;

	const int textLenght = (int)text.size();

	// if (textLenght == 0)
	// {
	//     textBenchmark->Stop();
	//     return;
	// }

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
	vertexDescriptorList.AddVertexDescriptor(VertexElements::UV_32_BITS);
#if defined(__vita__)
	vertexDescriptorList.AddVertexDescriptor(VertexElements::COLOR_4_FLOATS);
#endif
	vertexDescriptorList.AddVertexDescriptor(VertexElements::POSITION_32_BITS);


	std::shared_ptr<MeshData> mesh = MeshData::MakeMeshData();
	mesh->CreateSubMesh(6 * charCountToDraw, 6 * charCountToDraw, vertexDescriptorList);
	mesh->unifiedColor = color;

	int drawnCharIndex = 0;
	for (int i = 0; i < textLenght; i++)
	{
		const char c = text[i];
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
	subMesh->AddVertex(ch->uv.x, ch->uv.y, white, w + x, fixedY, 0, indice);
	subMesh->AddVertex(ch->uvOffet.x, ch->uv.y, white, x, fixedY, 0, 1 + indice);
	subMesh->AddVertex(ch->uvOffet.x, ch->uvOffet.y, white, x, h + fixedY, 0, 2 + indice);

	subMesh->AddVertex(ch->uv.x, ch->uv.y, white, w + x, fixedY, 0, 3 + indice);
	subMesh->AddVertex(ch->uv.x, ch->uvOffet.y, white, w + x, h + fixedY, 0, 4 + indice);
	subMesh->AddVertex(ch->uvOffet.x, ch->uvOffet.y, white, x, h + fixedY, 0, 5 + indice);
#else
	subMesh->AddVertex(ch->uv.x, ch->uv.y, w + x, fixedY, 0, indice);
	subMesh->AddVertex(ch->uvOffet.x, ch->uv.y, x, fixedY, 0, 1 + indice);
	subMesh->AddVertex(ch->uvOffet.x, ch->uvOffet.y, x, h + fixedY, 0, 2 + indice);

	subMesh->AddVertex(ch->uv.x, ch->uv.y, w + x, fixedY, 0, 3 + indice);
	subMesh->AddVertex(ch->uv.x, ch->uvOffet.y, w + x, h + fixedY, 0, 4 + indice);
	subMesh->AddVertex(ch->uvOffet.x, ch->uvOffet.y, x, h + fixedY, 0, 5 + indice);
#endif
	subMesh->SetIndex(indiceIndex + 0, indice + 0);
	subMesh->SetIndex(indiceIndex + 1, indice + 2);
	subMesh->SetIndex(indiceIndex + 2, indice + 1);
	subMesh->SetIndex(indiceIndex + 3, indice + 3);
	subMesh->SetIndex(indiceIndex + 4, indice + 4);
	subMesh->SetIndex(indiceIndex + 5, indice + 5);
}

TextInfo *TextManager::GetTextInfomations(const std::string &text, int textLen, std::shared_ptr<Font> font, float scale)
{
	TextInfo *textInfos = new TextInfo();
	if (!font || !font->GetFontAtlas())
		return textInfos;

	textInfos->linesInfo.emplace_back(LineInfo());

	int currentLine = 0;
	float higherY = 0;
	float lowerY = 0;

	for (int i = 0; i < textLen; i++)
	{
		const Character *ch = font->Characters[(int)text[i]];
		if (text[i] == '\n')
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
	}
	textInfos->linesInfo[currentLine].lenght *= scale;
	textInfos->linesInfo[currentLine].y1 = (higherY - lowerY) * scale;

	textInfos->maxLineHeight = font->maxCharHeight * scale;
	textInfos->lineCount = currentLine + 1;

	return textInfos;
}
