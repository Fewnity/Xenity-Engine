#include "text_renderer_canvas.h"

#include "text_manager.h"

#include <engine/graphics/graphics.h>
#include <engine/graphics/3d_graphics/mesh_data.h>

#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>
#include <engine/debug/debug.h>

#pragma region Constructors / Destructor

TextRendererCanvas::TextRendererCanvas()
{
	componentName = "TextRendererCanvas";

	AssetManager::AddReflection(this);
	material = AssetManager::unlitMaterial;
}

ReflectiveData TextRendererCanvas::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, text, "text", true);
	Reflective::AddVariable(reflectedVariables, font, "font", true);
	Reflective::AddVariable(reflectedVariables, material, "material", true);
	Reflective::AddVariable(reflectedVariables, horizontalAlignment, "horizontalAlignment", true);
	Reflective::AddVariable(reflectedVariables, verticalAlignment, "verticalAlignment", true);
	Reflective::AddVariable(reflectedVariables, fontSize, "fontSize", true);
	return reflectedVariables;
}

void TextRendererCanvas::OnReflectionUpdated()
{
	isTextInfoDirty = true;
	Graphics::isRenderingBatchDirty = true;
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
		this->font = font;
		isTextInfoDirty = true;
		Graphics::isRenderingBatchDirty = true;
	}
}

std::shared_ptr<Material> TextRendererCanvas::GetMaterial()
{
	return material;
}

void TextRendererCanvas::SetMaterial(std::shared_ptr<Material> _material)
{
	material = _material;
	Graphics::isRenderingBatchDirty = true;
}

void TextRendererCanvas::OnDisabled()
{
	Graphics::isRenderingBatchDirty = true;
}

void TextRendererCanvas::OnEnabled()
{
	Graphics::isRenderingBatchDirty = true;
}

void TextRendererCanvas::CreateRenderCommands(RenderBatch& renderBatch)
{
	if (!material || !font)
		return;

	RenderCommand command = RenderCommand();
	command.material = material;
	command.drawable = this;
	command.subMesh = nullptr;
	command.transform = GetTransform();
	command.isEnabled = GetIsEnabled() && GetGameObject()->GetLocalActive();

	renderBatch.uiCommands.push_back(command);
	renderBatch.uiCommandIndex++;
}

/// <summary>
/// Draw text
/// </summary>
void TextRendererCanvas::DrawCommand(const RenderCommand& renderCommand)
{
	if (isTextInfoDirty)
	{
		if (textInfo)
			delete textInfo;
		if (mesh)
			mesh.reset();
		textInfo = TextManager::GetTextInfomations(text, (int)text.size(), font, 1);
		mesh = TextManager::CreateMesh(text, textInfo, horizontalAlignment, verticalAlignment, color, font, fontSize);
		isTextInfoDirty = false;
	}
	TextManager::DrawText(text, textInfo, horizontalAlignment, verticalAlignment, GetTransform(), color, true, mesh, font, material);
}
