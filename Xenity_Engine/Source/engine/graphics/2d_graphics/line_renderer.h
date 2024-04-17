#pragma once
#include <engine/api.h>

class Texture;
class Material;
class MeshData;

#include <engine/graphics/iDrawable.h>

#include <engine/engine.h>
#include <engine/game_elements/gameplay_manager.h>
#include <engine/component.h>

#include <engine/vectors/vector3.h>
#include <engine/vectors/vector4.h>
#include <engine/graphics/color/color.h>

class API LineRenderer : public IDrawable
{
public:
	LineRenderer();
	~LineRenderer();

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated()  override;

	/**
	 * @brief [Internal]
	 */
	int GetDrawPriority() const override;

	void SetOrderInLayer(int orderInLayer);

	int GetOrderInLayer() const
	{
		return orderInLayer;
	}

	/**
	* @brief Set the color of the sprite
	*/
	void SetColor(const Color& color)
	{
		this->color = color;
	}

	Vector3 startPosition = Vector3(0, 0, 0);
	Vector3 endPosition = Vector3(0, 0, 0);

private:

	/**
	* @brief Called when the component is disabled
	*/
	void OnDisabled() override;

	/**
	* @brief Called when the component is enabled
	*/
	void OnEnabled() override;

	/**
	* @brief Create the render commands
	*/
	void CreateRenderCommands(RenderBatch& renderBatch) override;

	/**
	* @brief Draw the command
	*/
	void DrawCommand(const RenderCommand& renderCommand) override;

	Color color = Color();
	std::shared_ptr <MeshData> meshData = nullptr;
	std::shared_ptr <Material> material = nullptr;
	int orderInLayer = 0;

public:
	float width = 1;
};

