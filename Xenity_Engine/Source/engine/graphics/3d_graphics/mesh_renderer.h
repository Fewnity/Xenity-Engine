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
	void CreateRenderCommands(RenderBatch& renderBatch) override;
	void SetMeshData(std::shared_ptr <MeshData> meshData);
	std::shared_ptr <MeshData> GetMeshData();

	void OnDisabled() override;
	void OnEnabled() override;

private:
	int matCount = 0;
	//bool startBackup = false;
	std::shared_ptr <MeshData> meshData = nullptr;
	//std::vector<std::shared_ptr<Texture>> textures;
	std::vector<std::shared_ptr <Material>> materials;
	//std::vector<std::shared_ptr <Material>> backupMaterials;
	//void Draw() override;
	void DrawCommand(const RenderCommand& renderCommand) override;
};