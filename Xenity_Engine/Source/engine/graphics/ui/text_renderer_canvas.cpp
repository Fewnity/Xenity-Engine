#include "text_renderer_canvas.h"

#include "../../../xenity.h"
#include "../../graphics/3d_graphics/mesh_data.h"

#pragma region Constructors / Destructor

TextRendererCanvas::TextRendererCanvas()
{
	componentName = "TextRendererCanvas";
	type = Draw_UI;

	AssetManager::AddReflection(this);
	material = AssetManager::unlitMaterial;
}

std::unordered_map<std::string, ReflectionEntry> TextRendererCanvas::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	Reflection::AddVariable(reflectedVariables, text, "text", true);
	Reflection::AddVariable(reflectedVariables, font, "font", true);
	Reflection::AddVariable(reflectedVariables, material, "material", true);
	return reflectedVariables;
}

void TextRendererCanvas::OnReflectionUpdated()
{
	isTextInfoDirty = true;
}

TextRendererCanvas::~TextRendererCanvas()
{
	AssetManager::RemoveReflection(this);
}

int TextRendererCanvas::GetDrawPriority() const
{
	return 5000 + orderInLayer;
}

#pragma endregion

void TextRendererCanvas::SetOrderInLayer(int orderInLayer)
{
	this->orderInLayer = orderInLayer;
	Graphics::SetDrawOrderListAsDirty();
}

void TextRendererCanvas::SetText(const std::string& text)
{
	if (this->text != text)
	{
		this->text = text;
		isTextInfoDirty = true;
	}
}

void TextRendererCanvas::SetFont(const std::shared_ptr<Font>& font)
{
	if (this->font != font)
	{
		this->font = std::move(font);
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
				mesh.reset();
			textInfo = TextManager::GetTextInfomations(text, (int)text.size(), font, 1);
			mesh = TextManager::CreateMesh(text, textInfo, horizontalAlignment, verticalAlignment, color, font);
			isTextInfoDirty = false;
		}
		TextManager::DrawText(text, textInfo, horizontalAlignment, verticalAlignment, GetTransform(), color, true, mesh, font, material);
	}
}
