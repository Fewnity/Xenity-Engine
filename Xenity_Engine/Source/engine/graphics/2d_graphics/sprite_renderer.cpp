#include "sprite_renderer.h"

#include <engine/graphics/graphics.h>
#include <engine/graphics/material.h>
#include "sprite_manager.h"

#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>


#pragma region Constructors / Destructor

SpriteRenderer::SpriteRenderer()
{
	componentName = "SpriteRenderer";
	type = IDrawableTypes::Draw_2D;
	AssetManager::AddReflection(this);
	material = AssetManager::unlitMaterial;
}

ReflectiveData SpriteRenderer::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, color, "color", true);
	Reflective::AddVariable(reflectedVariables, texture, "texture", true);
	Reflective::AddVariable(reflectedVariables, material, "material", true);
	return reflectedVariables;
}

SpriteRenderer::~SpriteRenderer()
{
	AssetManager::RemoveReflection(this);
}

int SpriteRenderer::GetDrawPriority() const
{
	return orderInLayer;
}

void SpriteRenderer::SetOrderInLayer(int orderInLayer)
{
	this->orderInLayer = orderInLayer;
	Graphics::SetDrawOrderListAsDirty();
}

#pragma endregion

void SpriteRenderer::CreateRenderCommands(RenderBatch& renderBatch)
{
	/*if (!material->useTransparency)
	{
		RenderQueue& renderQueue = renderBatch.renderQueues[material->fileId];
		command = &renderQueue.commands[renderQueue.commandIndex];
		renderQueue.commandIndex++;
	}
	else
	{
		command = &renderBatch.transparentMeshCommands[renderBatch.transparentMeshCommandIndex];
		renderBatch.transparentMeshCommandIndex++;
	}*/
	if (!material || !texture)
		return;

	RenderCommand* command = nullptr;
	command = &renderBatch.spriteCommands[renderBatch.spriteCommandIndex];
	renderBatch.spriteCommandIndex++;

	command->material = material;
	command->drawable = this;
	command->subMesh = nullptr;
	command->transform = GetTransform();
	command->isEnabled = GetIsEnabled() && GetGameObject()->GetLocalActive();
}

/// <summary>
/// Draw sprite
/// </summary>
void SpriteRenderer::Draw()
{
	//if (const std::shared_ptr<GameObject> gameObject = GetGameObject())
	//{
	//	// Draw the sprite only if there is a texture and if the component/gameobject is active
	//	if (gameObject->GetLocalActive() && GetIsEnabled() && material)
	//	{
	//		material->texture = texture;
	//		SpriteManager::DrawSprite(GetTransform(), color, material);
	//	}
	//}
}

void SpriteRenderer::DrawSubMesh(const RenderCommand& renderCommand)
{
	SpriteManager::DrawSprite(GetTransform(), color, material, texture);
}
