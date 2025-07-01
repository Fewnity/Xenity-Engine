#include "image_renderer.h"

#include <engine/graphics/texture/texture.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>
#include <engine/graphics/2d_graphics/sprite_manager.h>
#include <engine/graphics/graphics.h>
#include <engine/debug/stack_debug_object.h>
#include <engine/inputs/input_system.h>
#include <engine/game_elements/rect_transform.h>
#include <engine/graphics/ui/canvas.h>

ReflectiveData ImageRenderer::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, m_image, "image", true);
	Reflective::AddVariable(reflectedVariables, color, "color", true);
	Reflective::AddVariable(reflectedVariables, m_orderInLayer, "orderInLayer", true);
	return reflectedVariables;
}

void ImageRenderer::OnReflectionUpdated()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	Graphics::s_isRenderingBatchDirty = true;
	Graphics::s_needUpdateUIOrdering = true;
}

void ImageRenderer::CreateRenderCommands(RenderBatch& renderBatch)
{
	if (!m_image)
		return;

	RenderCommand command = RenderCommand();
	command.material = AssetManager::unlitMaterial.get();
	command.drawable = this;
	command.subMesh = nullptr;
	command.transform = GetTransformRaw();
	command.isEnabled = IsEnabled() && GetGameObjectRaw()->IsLocalActive();

	renderBatch.uiCommands.push_back(command);
	renderBatch.uiCommandIndex++;
}

void ImageRenderer::DrawCommand(const RenderCommand& renderCommand)
{
	XASSERT(m_image, "[ImageRenderer::DrawCommand] Image is nullptr");

	SpriteManager::DrawSprite(*GetTransformRaw(), color, *AssetManager::unlitMaterial, m_image.get(), true);
}

void ImageRenderer::SetImage(const std::shared_ptr<Texture>& image)
{
	m_image = image;
	Graphics::s_isRenderingBatchDirty = true;
}

void ImageRenderer::OnDisabled()
{
	Graphics::s_isRenderingBatchDirty = true;
}

void ImageRenderer::OnEnabled()
{
	Graphics::s_isRenderingBatchDirty = true;
}
