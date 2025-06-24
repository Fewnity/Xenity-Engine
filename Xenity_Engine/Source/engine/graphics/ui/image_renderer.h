#pragma once

#include <engine/graphics/iDrawable.h>
#include <engine/graphics/color/color.h>

class Texture;

class API ImageRenderer : public IDrawable
{
public:
	Color color;

	/**
	* @brief Get the image texture
	*/
	[[nodiscard]] const std::shared_ptr<Texture>& GetIamge() const
	{
		return m_image;
	}

	/**
	* @brief Set the image texture
	*/
	void SetImage(const std::shared_ptr<Texture>& image);

private:
	std::shared_ptr<Texture> m_image;
	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;
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
};

