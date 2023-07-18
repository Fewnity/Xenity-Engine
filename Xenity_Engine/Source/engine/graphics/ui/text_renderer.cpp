#include "text_renderer.h"

#include "../../../xenity.h"
#include "../../graphics/3d_graphics/mesh_data.h"

#pragma region Constructors / Destructor

TextRenderer::TextRenderer()
{
	componentName = "TextRenderer";
	//SetReflection();
}

/*void TextRenderer::SetReflection()
{
	reflectedVariables["text"] = &text;
}*/

std::unordered_map<std::string, Variable> TextRenderer::GetReflection()
{
	std::unordered_map<std::string, Variable> reflectedVariables;
	reflectedVariables["text"] = text;
	return reflectedVariables;
}

// TextRenderer::TextRenderer(Font *font, float size, Shader *shader)
// {
// 	this->font = font;
// 	this->size = size;
// 	this->shader = shader;
// }

TextRenderer::~TextRenderer()
{
}

int TextRenderer::GetDrawPriority() const
{
	return orderInLayer;
}

#pragma endregion

void TextRenderer::SetText(std::string text)
{
	if (this->text != text)
	{
		this->text = text;
		isTextInfoDirty = true;
	}
}

void TextRenderer::SetFont(Font *font)
{
	if (this->font != font)
	{
		this->font = font;
		isTextInfoDirty = true;
	}
}

/// <summary>
/// Draw text
/// </summary>
void TextRenderer::Draw()
{
	if (GetGameObject()->GetLocalActive() && GetIsEnabled() /*&& shader != nullptr*/)
	{
		if (isTextInfoDirty)
		{
			if (textInfo)
				delete textInfo;
			if (mesh)
				delete mesh;
			textInfo = TextManager::GetTextInfomations(text, text.size(), font, 1);
			mesh = TextManager::CreateMesh(text, textInfo, horizontalAlignment, verticalAlignment, color, font);
			isTextInfoDirty = false;
		}
		TextManager::DrawText(text, textInfo, horizontalAlignment, verticalAlignment, GetTransform(), color, false, mesh, font);
	}
}
