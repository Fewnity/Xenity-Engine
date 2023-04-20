#pragma once

#include "iDrawable.h"
#include "../component.h"
#include <vector>
#include "../vectors/vector2.h"
#include "../vectors/vector4.h"
#include <glm/glm.hpp>


class Texture;
class Material;
class SpriteBatch;

class TileMap : public IDrawable
{
public:
	TileMap() = delete;
	TileMap(Material* material);
	class Tile 
	{
	public:
		int textureId = 0;
		glm::mat4 transformationMatrix;
		Vector2 vertices[4];
		SpriteBatch* batch = nullptr;
	private:
	};

	int GetDrawPriority();
	void Setup(int width, int height);
	Tile * GetTile(int x, int y);
	void SetTile(int x, int y, int textureId);
	int GetWidth();
	int GetHeight();
	void AddTexture(Texture* texture);
	void RemoveTexture(Texture* texture);
	Vector2 spritesScale = Vector2(1,1);

private:
	Material* material = nullptr;
	void Update();
	void Draw();

	int GetTextureIndex(Texture* texture);

	std::vector<Texture* > textures;
	int width = 0;
	int height = 0;
	Tile* tiles = nullptr;
	Vector4 color = Vector4(1, 1, 1, 1);
	std::vector<SpriteBatch*> SpriteBatches;
	bool needUpdateVertices = true;
};

