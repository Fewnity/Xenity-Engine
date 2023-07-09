#include "text_renderer_canvas.h"

#include "../../../xenity.h"
#include "../../graphics/3d_graphics/mesh_data.h"

#pragma region Constructors / Destructor

TextRendererCanvas::TextRendererCanvas()
{
	componentName = "Canvas Text Renderer";

	type = Draw_UI;
}

// TextRendererCanvas::TextRendererCanvas(Font *font, float size, Shader *shader)
// {
// 	this->font = font;
// 	this->size = size;
// 	this->shader = shader;
// }

TextRendererCanvas::~TextRendererCanvas()
{
}

int TextRendererCanvas::GetDrawPriority() const
{
	return 5000 + orderInLayer;
}

#pragma endregion

void TextRendererCanvas::SetText(std::string text)
{
	if (this->text != text)
	{
		this->text = text;
		isTextInfoDirty = true;
	}
}

void TextRendererCanvas::SetFont(Font *font)
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
void TextRendererCanvas::Draw()
{
	if (GetGameObject()->GetLocalActive() && GetIsEnabled() /* && shader != nullptr*/)
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
		TextManager::DrawText(text, textInfo, horizontalAlignment, verticalAlignment, GetTransform(), color, true, mesh, font);
	}
}
