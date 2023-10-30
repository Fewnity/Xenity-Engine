#include "text_renderer.h"

#include "../../../xenity.h"
#include "../../graphics/3d_graphics/mesh_data.h"

#pragma region Constructors / Destructor

TextRenderer::TextRenderer()
{
	componentName = "TextRenderer";
	type = Draw_3D;

	AssetManager::AddReflection(this);
	material = AssetManager::standardMaterial;
}

std::unordered_map<std::string, ReflectionEntry> TextRenderer::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	Reflection::AddVariable(reflectedVariables, text, "text", true);
	Reflection::AddVariable(reflectedVariables, font, "font", true);
	Reflection::AddVariable(reflectedVariables, material, "material", true);
	return reflectedVariables;
}

void TextRenderer::OnReflectionUpdated()
{
	isTextInfoDirty = true;
}

TextRenderer::~TextRenderer()
{
	AssetManager::RemoveReflection(this);
}

int TextRenderer::GetDrawPriority() const
{
	return orderInLayer;
}

void TextRenderer::SetOrderInLayer(int orderInLayer)
{
	this->orderInLayer = orderInLayer;
	Graphics::SetDrawOrderListAsDirty();
}

#pragma endregion

void TextRenderer::SetText(const std::string& text)
{
	if (this->text != text)
	{
		this->text = text;
		isTextInfoDirty = true;
	}
}

void TextRenderer::SetFont(const std::shared_ptr<Font>& font)
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
void TextRenderer::Draw()
{
	if (GetGameObject()->GetLocalActive() && GetIsEnabled() /*&& shader != nullptr*/)
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
		TextManager::DrawText(text, textInfo, horizontalAlignment, verticalAlignment, GetTransform(), color, false, mesh, font, material);
	}
}
