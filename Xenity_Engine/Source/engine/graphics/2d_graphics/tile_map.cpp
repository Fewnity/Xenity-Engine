#include "tile_map.h"
#include <malloc.h>
#include "../3d_graphics/mesh_manager.h"
#include "sprite_manager.h"
#include "../3d_graphics/mesh_data.h"
#include "../texture.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/ext/matrix_transform.hpp>

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
	this->width = width;
	this->height = height;

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
		if (dirtyMeshes)
		{
			dirtyMeshes = false;
			// Delete old meshes
			int meshSize = (int)meshes.size();
			for (int i = 0; i < meshSize; i++)
			{
				delete meshes[i];
				meshes.erase(meshes.begin() + i);
			}
			meshes.clear();

			// Create new empty meshes
			int textureSize = (int)textures.size();
			for (int i = 0; i < textureSize; i++)
			{
				MeshData *mesh = new MeshData(4 * width * height, 6 * width * height);
				mesh->index_count = 0;
				mesh->vertice_count = 0;
				mesh->isQuad = true;
				meshes.push_back(mesh);
			}

			// Fill meshes
			for (int x = 0; x < width; x++)
			{
				for (int y = 0; y < height; y++)
				{
					Tile *tile = GetTile(x, y);
					if (tile->textureId != 0)
					{
						MeshData *mesh = meshes[tile->textureId];
						int indiceOff = mesh->index_count;
						int verticeOff = mesh->vertice_count;

						mesh->AddVertice(0.0f, 1.0f, 0xFFFFFFFF, -0.5f - x, -0.5f + y, 0.0f, 0 + verticeOff);
						mesh->AddVertice(1.0f, 1.0f, 0xFFFFFFFF, 0.5f - x, -0.5f + y, 0.0f, 1 + verticeOff);
						mesh->AddVertice(1.0f, 0.0f, 0xFFFFFFFF, 0.5f - x, 0.5f + y, 0.0f, 2 + verticeOff);
						mesh->AddVertice(0.0f, 0.0f, 0xFFFFFFFF, -0.5f - x, 0.5f + y, 0.0f, 3 + verticeOff);

						mesh->indices[0 + indiceOff] = 0 + verticeOff;
						mesh->indices[1 + indiceOff] = 2 + verticeOff;
						mesh->indices[2 + indiceOff] = 1 + verticeOff;
						mesh->indices[3 + indiceOff] = 2 + verticeOff;
						mesh->indices[4 + indiceOff] = 0 + verticeOff;
						mesh->indices[5 + indiceOff] = 3 + verticeOff;

						mesh->index_count += 6;

						// mesh->indices[0 + indiceOff] = 0 + verticeOff;
						// mesh->indices[1 + indiceOff] = 2 + verticeOff;
						// mesh->indices[2 + indiceOff] = 1 + verticeOff;
						// mesh->indices[3 + indiceOff] = 2 + verticeOff;
						// mesh->indices[4 + indiceOff] = 0 + verticeOff;
						// mesh->indices[5 + indiceOff] = 3 + verticeOff;

						// mesh->index_count += 6;
						mesh->vertice_count += 4;
					}
				}
			}
		}

		int meshCount = meshes.size();
		for (int i = 0; i < meshCount; i++)
		{
			MeshManager::DrawMesh(Vector3(0), Vector3(0), Vector3(1), textures[i], meshes[i], false);
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
