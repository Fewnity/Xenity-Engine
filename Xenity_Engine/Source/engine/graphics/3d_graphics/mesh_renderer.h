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

	/**
	 * [Internal]
	 */
	int GetDrawPriority() const override;

	std::shared_ptr <MeshData> meshData = nullptr;
	std::vector< std::shared_ptr<Texture>> textures;
	std::shared_ptr <Material> material;
private:
	void Draw() override;
};