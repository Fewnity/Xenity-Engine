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
	this->chunkSize = chunkSize;

	if (tiles != nullptr)
		free(tiles);

	AddTexture(nullptr);

	tiles = (Tile *)malloc(width * height * sizeof(Tile));

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			Tile *tile = GetTile(x, y);
			tile->textureId = 0;
		}
	}

	int chunkCount = ceil(width / (float)chunkSize);
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
	needUpdateVertices = true;
	Tile *tile = GetTile(x, y);
	if (tile)
	{
		tile->textureId = textureId;
		dirtyMeshes = true;
	}
}

int Tilemap::GetWidth()
{
	return width;
}

int Tilemap::GetHeight()
{
	return height;
}

void Tilemap::Draw()
{
	if (GetGameObject()->GetLocalActive() && GetIsEnabled())
	{
		int textureSize = (int)textures.size() - 1;
		int chunkCount = ceil(width / (float)chunkSize);
		if (dirtyMeshes)
		{
			dirtyMeshes = false;
			for (int x = 0; x < chunkCount; x++)
			{
				for (int y = 0; y < chunkCount; y++)
				{
					int meshSize = (int)chunks[x + y * chunkCount]->meshes.size();
					for (int i = 0; i < meshSize; i++)
					{
						delete chunks[x + y * chunkCount]->meshes[i];
					}
					chunks[x + y * chunkCount]->meshes.clear();
				}
			}

			for (int x = 0; x < chunkCount; x++)
			{
				for (int y = 0; y < chunkCount; y++)
				{
					for (int i = 0; i < textureSize; i++)
					{
						MeshData *mesh = new MeshData(6 * chunkSize * chunkSize, 6 * chunkSize * chunkSize); //////////////////////////////////////////CHANGE BY CHUNK SIZE
						mesh->index_count = 0;
						mesh->vertice_count = 0;
						mesh->hasIndices = useIndices;
						chunks[x + y * chunkCount]->meshes.push_back(mesh);
					}
				}
			}

			// Fill meshes
			for (int x = 0; x < width; x++)
			{
				for (int y = 0; y < height; y++)
				{
					Tile *tile = GetTile(x, y);
					if (tile->textureId != 0)
					{
						int xChunk = floor(x / (float)chunkSize);
						int yChunk = floor(y / (float)chunkSize);
						MeshData *mesh = chunks[xChunk + yChunk * chunkCount]->meshes[tile->textureId - 1];
						int indiceOff = mesh->index_count;
						int verticeOff = mesh->vertice_count;

						if (!useIndices)
						{
							mesh->AddVertice(1.0f, 1.0f, 0xFFFFFFFF, -0.5f - x, -0.5f + y, 0.0f, 0 + verticeOff);
							mesh->AddVertice(0.0f, 0.0f, 0xFFFFFFFF, 0.5f - x, 0.5f + y, 0.0f, 1 + verticeOff);
							mesh->AddVertice(0.0f, 1.0f, 0xFFFFFFFF, 0.5f - x, -0.5f + y, 0.0f, 2 + verticeOff);

							mesh->AddVertice(0.0f, 0.0f, 0xFFFFFFFF, 0.5f - x, 0.5f + y, 0.0f, 3 + verticeOff);
							mesh->AddVertice(1.0f, 1.0f, 0xFFFFFFFF, -0.5f - x, -0.5f + y, 0.0f, 4 + verticeOff);
							mesh->AddVertice(1.0f, 0.0f, 0xFFFFFFFF, -0.5f - x, 0.5f + y, 0.0f, 5 + verticeOff);

							mesh->vertice_count += 6;
						}
						else
						{
							mesh->AddVertice(1.0f, 1.0f, 0xFFFFFFFF, -0.5f - x, -0.5f + y, 0.0f, 0 + verticeOff);
							mesh->AddVertice(0.0f, 1.0f, 0xFFFFFFFF, 0.5f - x, -0.5f + y, 0.0f, 1 + verticeOff);
							mesh->AddVertice(0.0f, 0.0f, 0xFFFFFFFF, 0.5f - x, 0.5f + y, 0.0f, 2 + verticeOff);
							mesh->AddVertice(1.0f, 0.0f, 0xFFFFFFFF, -0.5f - x, 0.5f + y, 0.0f, 3 + verticeOff);

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
		Vector3 pos = Graphics::usedCamera->GetTransform()->GetPosition();

		float s = Graphics::usedCamera->GetProjectionSize() * Window::GetAspectRatio() + chunkSize;
		float s2 = Graphics::usedCamera->GetProjectionSize() + chunkSize;

		for (int x = 0; x < chunkCount; x++)
		{
			if (x * (float)chunkSize <= pos.x + s && x * (float)chunkSize >= pos.x - s)
			{
				for (int y = 0; y < chunkCount; y++)
				{
					if (y * (float)chunkSize <= pos.y + s2 && y * (float)chunkSize >= pos.y - s2)
					{

						for (int textureI = 0; textureI < textureSize; textureI++)
						{
							MeshManager::DrawMesh(GetTransform()->GetPosition(), GetTransform()->GetRotation(), GetTransform()->GetLocalScale(), textures[textureI + 1], chunks[x + y * chunkCount]->meshes[textureI], false);
						}
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
	}
}
