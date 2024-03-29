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

	std::shared_ptr <MeshData> meshData = nullptr;
	std::vector< std::shared_ptr<Texture>> textures;
	std::vector<std::shared_ptr <Material>> materials;
	int matCount = 0;
	bool startBackup = false;
	std::vector<std::shared_ptr <Material>> backupMaterials;
	bool culled = false;
	void CreateRenderCommands(RenderBatch& renderBatch) override;
private:
	void Draw() override;
	void DrawSubMesh(const RenderCommand& renderCommand) override;
	void Update() override;
};