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

	bool culled = false;
	void SetMeshData(std::shared_ptr <MeshData> meshData);
	std::shared_ptr <MeshData> GetMeshData();


private:
	void OnDisabled() override;
	void OnEnabled() override;
	void CreateRenderCommands(RenderBatch& renderBatch) override;
	void DrawCommand(const RenderCommand& renderCommand) override;
	int matCount = 0;
	std::shared_ptr <MeshData> meshData = nullptr;
	std::vector<std::shared_ptr <Material>> materials;
};