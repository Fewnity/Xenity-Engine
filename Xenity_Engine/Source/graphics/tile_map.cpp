#include "tile_map.h"
#include <corecrt_malloc.h>
#include "../debug/debug.h"
#include "sprite_manager.h"
#include "texture.h"
#include "material.h"
#include <glm/ext/matrix_transform.hpp>
#include "../tools/profiler_benchmark.h"
#include <SDL2/SDL_stdinc.h>
#include "graphics.h"

TileMap::TileMap(Material* material)
{
	this->material = material;
}

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
			tile->batch = nullptr;
			//tile->spriteRenderer = new SpriteRendererNoTransform();
			//tile->spriteRenderer->material = material;
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
	if (tiles == nullptr || x < 0 || y < 0 || x >= height || y >= height )
		return nullptr;

	return &tiles[x * height + y];
}

void TileMap::SetTile(int x, int y, int textureId)
{
	needUpdateVertices = true;
	Tile* tile = GetTile(x, y);
	if (tile)
	{
		if (tile->batch != nullptr) {
			tile->batch->verticesCount -= 6;
		}
		tile->textureId = textureId;
		int batchCount = SpriteBatches.size();
		for (int i = 0; i < batchCount; i++)
		{
			if (SpriteBatches[i]->texture == textures[tile->textureId]) 
			{
				tile->batch = SpriteBatches[i];
				tile->batch->verticesCount += 6;
				break;
			}
		}

		if (textures[tile->textureId] != nullptr) 
		{
		float unitCoef = 100.0f / textures[tile->textureId]->GetPixelPerUnit();
		float w = textures[tile->textureId]->GetWidth() * unitCoef;
		float h = textures[tile->textureId]->GetHeight() * unitCoef;

		float newX = tile->transformationMatrix[3].x;
		float newY = tile->transformationMatrix[3].y;
		newX *= 100;
		newY *= 100;

		float xpos1 = newX;
		float xpos2 = newX + w;
		float ypos1 = newY;
		float ypos2 = newY + h;

		xpos1 -= w / 2.0f;
		xpos2 -= w / 2.0f;
		ypos1 -= h / 2.0f;
		ypos2 -= h / 2.0f;

		Vector2 c1 = Vector2(xpos1, ypos2);
		Vector2 c2 = Vector2(xpos1, ypos1);
		Vector2 c3 = Vector2(xpos2, ypos1);
		Vector2 c4 = Vector2(xpos2, ypos2);
		float cx = (c1.x + c2.x + c3.x + c4.x) / 4.0f;
		float cy = (c1.y + c2.y + c3.y + c4.y) / 4.0f;

		float angle = 0;
		float angleRad = angle * M_PI / 180.0f;

		float angleCos = cos(angleRad);
		float angleSin = sin(angleRad);
		Vector2 c1n = Vector2(cx + (c1.x - cx) * angleCos - (c1.y - cy) * angleSin, cy + (c1.x - cx) * angleSin + (c1.y - cy) * angleCos);
		Vector2 c2n = Vector2(cx + (c2.x - cx) * angleCos - (c2.y - cy) * angleSin, cy + (c2.x - cx) * angleSin + (c2.y - cy) * angleCos);
		Vector2 c3n = Vector2(cx + (c3.x - cx) * angleCos - (c3.y - cy) * angleSin, cy + (c3.x - cx) * angleSin + (c3.y - cy) * angleCos);
		Vector2 c4n = Vector2(cx + (c4.x - cx) * angleCos - (c4.y - cy) * angleSin, cy + (c4.x - cx) * angleSin + (c4.y - cy) * angleCos);

		tile->vertices[0] = c1n;
		tile->vertices[1] = c2n;
		tile->vertices[2] = c3n;
		tile->vertices[3] = c4n;
		}
		//tile->spriteRenderer->texture = textures[tile->textureId];
	}
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
	int batchCount = SpriteBatches.size();
	for (int i = 0; i < batchCount; i++)
	{
		SpriteBatches[i]->SetBatchSize();
	}
	if (tiles)
	{
		if (needUpdateVertices) {
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				Tile* tile = GetTile(x, y);
				if (textures[tile->textureId] != nullptr) 
				{
				//SpriteManager::RenderSprite(tile->transformationMatrix,
						//color, textures[tile->textureId], material);
				}
				if(tile->batch != nullptr)
				{
					tile->batch->AddVertices(tile->vertices);
				}
					
				//SpriteManager::AddToBatch(textures[1], tile->transformationMatrix, x * height + y, tile->transformationMatrix[3].x, tile->transformationMatrix[3].y, 1, 1);

				//SpriteManager::AddToBatch(x * height + y, tile->vertices);
			}
		}
		//SpriteManager::DrawBatch(textures[1], color, material);
		}
	}
	needUpdateVertices = false;
	for (int i = 0; i < batchCount; i++)
	{
		//SpriteBatches[i]->index = 0;
		SpriteBatches[i]->Draw(color);
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
		if (texture != nullptr) {
		SpriteBatch * newBatch = new SpriteBatch(material, texture);
		SpriteBatches.push_back(newBatch);
		}
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
