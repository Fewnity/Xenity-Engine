#include "tile_map.h"
#include <corecrt_malloc.h>
#include "../debug/debug.h"
#include "sprite_manager.h"
#include "texture.h"
#include "material.h"
#include <glm/ext/matrix_transform.hpp>


void TileMap::Setup(int width, int height)
{
	AddTexture(nullptr);

	this->width = width;
	this->height = height;
	if (tiles != nullptr)
		free(tiles);

	tiles = (Tile*)malloc(width * height * sizeof(Tile));

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			Tile* tile = GetTile(x, y);
			tile->textureId = 0;
			tile->transformationMatrix = glm::mat4(1.0f);
			tile->transformationMatrix = gameObject->transform.transformationMatrix;

			//tile->transformationMatrix *= gameObject->transform.transformationMatrix;

			tile->transformationMatrix = glm::translate(tile->transformationMatrix, glm::vec3(x, y, 0));
			//tile->transformationMatrix = glm::rotate(tile->transformationMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));//y
			//tile->transformationMatrix = glm::rotate(tile->transformationMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));//x
			//tile->transformationMatrix = glm::rotate(tile->transformationMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));//z
			//tile->transformationMatrix = glm::scale(tile->transformationMatrix, glm::vec3(1, 1, 1));
		}
	}
}

TileMap::Tile* TileMap::GetTile(int x, int y)
{
	if (tiles == nullptr)
		return nullptr;

	return &tiles[x * height + y];
}

void TileMap::SetTile(int x, int y, int textureId)
{
	Tile* tile = GetTile(x, y);
	tile->textureId = textureId;
}

int TileMap::GetWidth()
{
	return width;
}

int TileMap::GetHeight()
{
	return height;
}

void TileMap::Update() 
{
	/*for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			Tile* tile = GetTile(x, y);
			tile->textureId = 0;
			tile->transformationMatrix = glm::mat4(1.0f);
			tile->transformationMatrix = gameObject->transform.transformationMatrix;

			//tile->transformationMatrix *= gameObject->transform.transformationMatrix;

			tile->transformationMatrix = glm::translate(tile->transformationMatrix, glm::vec3(x, y, 0));
		}
	}*/
}

void TileMap::Draw()
{
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			Tile* tile = GetTile(x, y);
			if(textures[tile->textureId] != nullptr)
			SpriteManager::RenderSprite(tile->transformationMatrix,
				textures[tile->textureId], material);
		}
	}
}

int TileMap::GetTextureIndex(Texture* texture)
{
	int textureIndex = -1;
	int textureCount = textures.size();
	for (int i = 0; i < textureCount; i++)
	{
		if (textures[i] == texture) {
			textureIndex = i;
			break;
		}
	}
	return textureIndex;
}

void TileMap::AddTexture(Texture* texture)
{
	if (GetTextureIndex(texture) == -1)
	{
		textures.push_back(texture);
	}
}

void TileMap::RemoveTexture(Texture* texture)
{
	int index = GetTextureIndex(texture);
	if (index != -1)
	{
		textures.erase(textures.begin() + index);
	}
}
