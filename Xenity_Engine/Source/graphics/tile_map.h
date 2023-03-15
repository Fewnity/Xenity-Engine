#pragma once

#include "iDrawable.h"
#include "../component.h"
#include <vector>
#include "../vectors/vector2.h"
#include "spriteRenderer.h"
#include <glm/glm.hpp>

class Tile 
{
public:
	int textureId = 0;
	glm::mat4 transformationMatrix;
private:
};

class Texture;
class Material;

class TileMap : public Component, public IDrawable
{
public:
	void Setup(int width, int height);
	Tile * GetTile(int x, int y);
	void SetTile(int x, int y, int textureId);
	int GetWidth();
	int GetHeight();
	void Draw();	
	void AddTexture(Texture* texture);
	void RemoveTexture(Texture* texture);
	Vector2 spritesScale = Vector2(1,1);
	Material* material = nullptr;

private:
	SpriteRenderer* spriteRenderer = new SpriteRenderer();
	GameObject* gameObjectTileMap = new GameObject("@");

	int GetTextureIndex(Texture* texture);

	std::vector<Texture* > textures;
	int width = 0;
	int height = 0;
	Tile* tiles = nullptr;

};

