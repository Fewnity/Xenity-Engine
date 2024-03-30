#include "text_renderer.h"

#include "text_manager.h"

#include <engine/graphics/graphics.h>
#include <engine/graphics/3d_graphics/mesh_data.h>

#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>


#pragma region Constructors / Destructor

TextRenderer::TextRenderer()
{
	componentName = "TextRenderer";
	type = IDrawableTypes::Draw_3D;

	AssetManager::AddReflection(this);
	material = AssetManager::standardMaterial;
}

ReflectiveData TextRenderer::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, text, "text", true);
	Reflective::AddVariable(reflectedVariables, font, "font", true);
	Reflective::AddVariable(reflectedVariables, material, "material", true);
	Reflective::AddVariable(reflectedVariables, horizontalAlignment, "horizontalAlignment", true);
	Reflective::AddVariable(reflectedVariables, verticalAlignment, "verticalAlignment", true);
	return reflectedVariables;
}

void TextRenderer::OnReflectionUpdated()
{
	isTextInfoDirty = true;
	Graphics::isRenderingBatchDirty = true;
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
		this->font = font;
		isTextInfoDirty = true;
	}
}

void TextRenderer::OnDisabled()
{
	Graphics::isRenderingBatchDirty = true;
}

void TextRenderer::OnEnabled()
{
	Graphics::isRenderingBatchDirty = true;
}

void TextRenderer::CreateRenderCommands(RenderBatch& renderBatch)
{
	if (!material || !font)
		return;

	RenderCommand command = RenderCommand();
	command.material = material;
	command.drawable = this;
	command.subMesh = nullptr;
	command.transform = GetTransform();
	command.isEnabled = GetIsEnabled() && GetGameObject()->GetLocalActive();

	renderBatch.transparentMeshCommands.push_back(command);
	renderBatch.transparentMeshCommandIndex++;
}

/// <summary>
/// Draw text
/// </summary>
void TextRenderer::DrawCommand(const RenderCommand& renderCommand)
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
