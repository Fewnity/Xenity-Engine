#pragma once
#include <engine/api.h>

#include <engine/graphics/iDrawable.h>
#include <vector>
#include <memory>

class MeshData;
class Texture;
class Material;

class API MeshRenderer : public IDrawable
{
public:
	MeshRenderer();
	~MeshRenderer();

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	/**
	 * [Internal]
	 */
	int GetDrawPriority() const override;

	/**
	* Set mesh data
	* @param meshData The mesh data
	*/
	void SetMeshData(std::shared_ptr <MeshData> meshData);

	/**
	* Get mesh data
	*/
	std::shared_ptr <MeshData> GetMeshData();

private:

	/**
	* Called when the component is disabled
	*/
	void OnDisabled() override;

	/**
	* Called when the component is enabled
	*/
	void OnEnabled() override;

	/**
	* Create the render commands
	*/
	void CreateRenderCommands(RenderBatch& renderBatch) override;

	/**
	* Draw the command
	*/
	void DrawCommand(const RenderCommand& renderCommand) override;

	std::shared_ptr <MeshData> meshData = nullptr;
	std::vector<std::shared_ptr <Material>> materials;
	int matCount = 0;

public:
	bool culled = false;
};