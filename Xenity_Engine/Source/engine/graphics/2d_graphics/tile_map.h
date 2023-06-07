#pragma once

#include "../iDrawable.h"
#include <vector>
#include "../../vectors/vector2.h"
#include "../../vectors/vector4.h"
#include <glm/glm.hpp>
#include "../color/color.h"
#include "../../game_elements/gameobject.h"
#include "../../engine.h"

class Texture;
class MeshData;
// class Material;
// class SpriteBatch;

class Tilemap : public IDrawable
{
public:
	Tilemap();

	class Tile
	{
	public:
		int textureId = 0;
		// glm::mat4 transformationMatrix;
		// Vector2 vertices[4];
		// SpriteBatch *batch = nullptr;

	private:
	};

	int GetDrawPriority() const;
	void Setup(int width, int height);
	Tile *GetTile(int x, int y);
	void SetTile(int x, int y, Texture *textureId);
	void SetTile(int x, int y, int textureId);
	int GetWidth();
	int GetHeight();
	void AddTexture(Texture *texture);
	void RemoveTexture(Texture *texture);
	Vector2 spritesScale = Vector2(1, 1);
	bool dirtyMeshes = false;
	// Material *material = nullptr;

	void SetOrderInLayer(int orderInLayer)
	{
		this->orderInLayer = orderInLayer;
		Engine::drawOrderListDirty = true;
	}

	int GetOrderInLayer() const
	{
		return orderInLayer;
	}

private:
	int orderInLayer = 0;
	void Draw();

	int GetTextureIndex(Texture *texture);

	std::vector<Texture *> textures;
	std::vector<MeshData *> meshes;
	int width = 0;
	int height = 0;
	Tile *tiles = nullptr;
	Color color = Color();
	// std::vector<SpriteBatch *> spriteBatches;
	bool needUpdateVertices = true;
};
