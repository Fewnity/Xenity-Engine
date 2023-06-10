#include "tile_map.h"
#include <malloc.h>
#include "../../../xenity.h"
#include "../3d_graphics/mesh_manager.h"
#include "sprite_manager.h"
#include "../3d_graphics/mesh_data.h"
#include "../texture.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/ext/matrix_transform.hpp>

#define DEFAULT_CHUNK_SIZE 10

Tilemap::Tilemap()
{
	componentName = "Tilemap";
}

int Tilemap::GetDrawPriority() const
{
	return orderInLayer;
}

void Tilemap::Setup(int width, int height)
{
	Setup(width, height, DEFAULT_CHUNK_SIZE);
}

void Tilemap::Setup(int width, int height, int chunkSize)
{
	this->width = width;
	this->height = height;

	// Verify chunk size
	if (chunkSize <= 0)
	{
		chunkSize = 1;
	}
	else if (chunkSize > width || chunkSize > height)
	{
		chunkSize = std::min(width, height);
	}

	this->chunkSize = chunkSize;

	if (tiles != nullptr)
		free(tiles);

	// Add empty texture to make empty tiles
	AddTexture(nullptr);

	// Alloc tiles and set texture to empty
	tiles = (Tile *)malloc(width * height * sizeof(Tile));

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			Tile *tile = GetTile(x, y);
			tile->textureId = 0;
		}
	}

	// Create chunk
	chunkCount = ceil(width / (float)chunkSize);
	for (int x = 0; x < chunkCount; x++)
	{
		for (int y = 0; y < chunkCount; y++)
		{
			TilemapChunk *chunk = new TilemapChunk();
			chunks.push_back(chunk);
		}
	}
}

Tilemap::Tile *Tilemap::GetTile(int x, int y)
{
	if (tiles == nullptr || x < 0 || y < 0 || x >= height || y >= height)
		return nullptr;

	return &tiles[x * height + y];
}

void Tilemap::SetTile(int x, int y, Texture *texture)
{
	int textureSize = (int)textures.size();
	for (int i = 0; i < textureSize; i++)
	{
		if (textures[i] == texture)
		{
			SetTile(x, y, i);
			break;
		}
	}
}

void Tilemap::SetTile(int x, int y, int textureId)
{
	Tile *tile = GetTile(x, y);
	if (tile)
	{
		needUpdateVertices = true;
		tile->textureId = textureId;
		dirtyMeshes = true;
	}
}

/**
 * @brief Fill all chunks meshes
 *
 */
void Tilemap::FillChunks()
{
	// Fill meshes
	for (int x = 0; x < width; x++)
	{
		int xChunk = floor(x / (float)chunkSize);
		for (int y = 0; y < height; y++)
		{
			Tile *tile = GetTile(x, y);
			if (tile->textureId != 0)
			{
				int yChunk = floor(y / (float)chunkSize);
				MeshData *mesh = chunks[xChunk + yChunk * chunkCount]->meshes[tile->textureId - 1];
				int indiceOff = mesh->index_count;
				int verticeOff = mesh->vertice_count;

				if (!useIndices)
				{
					// Create tile with vertices only
					mesh->AddVertex(1.0f, 1.0f, -0.5f - x, -0.5f + y, 0.0f, 0 + verticeOff);
					mesh->AddVertex(0.0f, 0.0f, 0.5f - x, 0.5f + y, 0.0f, 1 + verticeOff);
					mesh->AddVertex(0.0f, 1.0f, 0.5f - x, -0.5f + y, 0.0f, 2 + verticeOff);

					mesh->AddVertex(0.0f, 0.0f, 0.5f - x, 0.5f + y, 0.0f, 3 + verticeOff);
					mesh->AddVertex(1.0f, 1.0f, -0.5f - x, -0.5f + y, 0.0f, 4 + verticeOff);
					mesh->AddVertex(1.0f, 0.0f, -0.5f - x, 0.5f + y, 0.0f, 5 + verticeOff);

					mesh->vertice_count += 6;
				}
				else
				{
					// Create tile with vertices and indices
					mesh->AddVertex(1.0f, 1.0f, -0.5f - x, -0.5f + y, 0.0f, 0 + verticeOff);
					mesh->AddVertex(0.0f, 1.0f, 0.5f - x, -0.5f + y, 0.0f, 1 + verticeOff);
					mesh->AddVertex(0.0f, 0.0f, 0.5f - x, 0.5f + y, 0.0f, 2 + verticeOff);
					mesh->AddVertex(1.0f, 0.0f, -0.5f - x, 0.5f + y, 0.0f, 3 + verticeOff);

					mesh->indices[0 + indiceOff] = 0 + verticeOff;
					mesh->indices[1 + indiceOff] = 2 + verticeOff;
					mesh->indices[2 + indiceOff] = 1 + verticeOff;
					mesh->indices[3 + indiceOff] = 2 + verticeOff;
					mesh->indices[4 + indiceOff] = 0 + verticeOff;
					mesh->indices[5 + indiceOff] = 3 + verticeOff;
					mesh->index_count += 6;
					mesh->vertice_count += 4;
				}
			}
		}
	}
}

/**
 * @brief Delete and create new meshes for each chunk
 *
 */
void Tilemap::CreateChunksMeshes()
{
	Color color = Color::CreateFromRGBA(0, 255, 255, 255);

	// Set vertices and indices per tile
	int verticesPerTile = 4;
	int indicesPerTile = 6;
	if (!useIndices)
	{
		verticesPerTile = 6;
		indicesPerTile = 0;
	}

	for (int x = 0; x < chunkCount; x++)
	{
		for (int y = 0; y < chunkCount; y++)
		{
			TilemapChunk *chunk = chunks[x + y * chunkCount];

			// Delete chunk meshes
			int meshSize = (int)chunk->meshes.size();
			for (int i = 0; i < meshSize; i++)
			{
				delete chunk->meshes[i];
			}
			chunk->meshes.clear();

			// Create new meshes
			for (int i = 0; i < textureSize; i++)
			{
				MeshData *mesh = new MeshData(verticesPerTile * chunkSize * chunkSize, indicesPerTile * chunkSize * chunkSize, false);
				mesh->index_count = 0;
				mesh->vertice_count = 0;
				mesh->hasIndices = useIndices;
				mesh->unifiedColor = color;
				chunk->meshes.push_back(mesh);
			}
		}
	}
}

void Tilemap::Draw()
{
	if (GetGameObject()->GetLocalActive() && GetIsEnabled())
	{
		if (dirtyMeshes)
		{
			dirtyMeshes = false;

			CreateChunksMeshes();
			FillChunks();
		}

		DrawChunks();
	}
}

/**
 * @brief Draw visible chunks
 *
 */
void Tilemap::DrawChunks()
{
	Vector3 cameraPos = Graphics::usedCamera->GetTransform()->GetPosition();

	float xArea = Graphics::usedCamera->GetProjectionSize() * Window::GetAspectRatio() + chunkSize;
	float yArea = Graphics::usedCamera->GetProjectionSize() + chunkSize;

	float xChunkPosition;
	float yChunkPosition;

	// For each chunk, check if the camera can see it
	for (int x = 0; x < chunkCount; x++)
	{
		xChunkPosition = x * (float)chunkSize;
		if (xChunkPosition <= cameraPos.x + xArea && xChunkPosition >= cameraPos.x - xArea)
		{
			for (int y = 0; y < chunkCount; y++)
			{
				yChunkPosition = y * (float)chunkSize;
				if (yChunkPosition <= cameraPos.y + yArea && yChunkPosition >= cameraPos.y - yArea)
				{
					// Draw each texture
					for (int textureI = 0; textureI < textureSize; textureI++)
					{
						MeshManager::DrawMesh(GetTransform()->GetPosition(), GetTransform()->GetRotation(), GetTransform()->GetLocalScale(), textures[textureI + 1], chunks[x + y * chunkCount]->meshes[textureI], false);
					}
				}
			}
		}
	}
}

int Tilemap::GetTextureIndex(Texture *texture)
{
	int textureIndex = -1;
	int textureCount = (int)textures.size();
	for (int i = 0; i < textureCount; i++)
	{
		if (textures[i] == texture)
		{
			textureIndex = i;
			break;
		}
	}
	return textureIndex;
}

void Tilemap::AddTexture(Texture *texture)
{
	if (GetTextureIndex(texture) == -1)
	{
		textures.push_back(texture);
		textureSize = (int)textures.size() - 1;
	}
}

void Tilemap::RemoveTexture(Texture *texture)
{
	int index = GetTextureIndex(texture);
	if (index != -1)
	{
		// delete meshes[index];
		// meshes.erase(meshes.begin() + index);
		textures.erase(textures.begin() + index);
		textureSize = (int)textures.size() - 1;
	}
}
