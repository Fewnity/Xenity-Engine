#include "image_renderer.h"

#include <engine/graphics/texture/texture.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>
#include <engine/graphics/2d_graphics/sprite_manager.h>
#include <engine/graphics/graphics.h>
#include <engine/debug/stack_debug_object.h>

ReflectiveData ImageRenderer::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, image, "image", true);
	Reflective::AddVariable(reflectedVariables, color, "color", true);
	return reflectedVariables;
}

void ImageRenderer::OnReflectionUpdated()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	Graphics::s_isRenderingBatchDirty = true;
}

void ImageRenderer::CreateRenderCommands(RenderBatch& renderBatch)
{
	if (!image)
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
	SpriteManager::DrawSprite(*GetTransformRaw(), color, *AssetManager::unlitMaterial, image.get(), true);
}
