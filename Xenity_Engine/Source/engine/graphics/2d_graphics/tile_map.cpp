// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(ENABLE_EXPERIMENTAL_FEATURES)

#include "tile_map.h"

#define _USE_MATH_DEFINES
#if defined(__PSP__) || defined(__vita__)
#undef __STRICT_ANSI__
#endif
#include <math.h>
#include <malloc.h>

#include <engine/graphics/graphics.h>
#include <engine/graphics/texture.h>
#include <engine/graphics/camera.h>
#include <engine/graphics/3d_graphics/mesh_manager.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include "sprite_manager.h"


#define DEFAULT_CHUNK_SIZE 10

Tilemap::Tilemap()
{
	AssetManager::AddReflection(this);
}

Tilemap::~Tilemap()
{
	AssetManager::RemoveReflection(this);
}

ReflectiveData Tilemap::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, width, "width", true);
	Reflective::AddVariable(reflectedVariables, height, "height", true);
	Reflective::AddVariable(reflectedVariables, color, "color", true);
	return reflectedVariables;
}

void Tilemap::Setup(int _width, int _height)
{
	Setup(_width, _height, DEFAULT_CHUNK_SIZE);
}

void Tilemap::Setup(int _width, int _height, int _chunkSize)
{
	this->width = _width;
	this->height = _height;

	// Verify chunk size
	if (_chunkSize <= 0)
	{
		_chunkSize = 1;
	}
	else if (_chunkSize > _width || _chunkSize > _height)
	{
		_chunkSize = std::min(_width, _height);
	}

	this->chunkSize = _chunkSize;

	free(tiles);

	// Add empty texture to make empty tiles
	AddTexture(nullptr);

	// Alloc tiles and set texture to empty
	tiles = (Tile*)malloc((size_t)_width * _height * sizeof(Tile));

	for (int x = 0; x < _width; x++)
	{
		for (int y = 0; y < _height; y++)
		{
			Tile* tile = GetTile(x, y);
			tile->textureId = 0;
		}
	}

	// Create chunk
	chunkCount = (int)ceil(_width / (float)_chunkSize);
	for (int x = 0; x < chunkCount; x++)
	{
		for (int y = 0; y < chunkCount; y++)
		{
			TilemapChunk* chunk = new TilemapChunk();
			chunks.push_back(chunk);
		}
	}
}

Tilemap::Tile* Tilemap::GetTile(int x, int y) const
{
	if (tiles == nullptr || x < 0 || y < 0 || x >= height || y >= height)
		return nullptr;

	return &tiles[x * height + y];
}

void Tilemap::SetTile(int x, int y, Texture* texture)
{
	int _textureSize = (int)textures.size();
	for (int i = 0; i < _textureSize; i++)
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
	Tile* tile = GetTile(x, y);
	// If the tile exists
	if (tile)
	{
		needUpdateVertices = true;
		tile->textureId = textureId;
		dirtyMeshes = true;
	}
}

void Tilemap::FillChunks()
{
	// Fill meshes
	for (int x = 0; x < width; x++)
	{
		int xChunk = (int)floor(x / (float)chunkSize);
		for (int y = 0; y < height; y++)
		{
			Tile* tile = GetTile(x, y);
			if (tile->textureId != 0)
			{
				int yChunk = (int)floor(y / (float)chunkSize);
				MeshData* mesh = chunks[(size_t)xChunk + (size_t)yChunk * chunkCount]->meshes[(size_t)tile->textureId - 1];
				std::unique_ptr<MeshData::SubMesh>& subMesh = mesh->m_subMeshes[0];

				int indiceOff = subMesh->index_count;
				int verticeOff = subMesh->vertice_count;

				float unitCoef = 100.0f / textures[tile->textureId]->GetPixelPerUnit();
				float w = textures[tile->textureId]->GetWidth() * unitCoef;
				float h = textures[tile->textureId]->GetHeight() * unitCoef;
				Vector2 spriteSize = Vector2(0.5f * w / 100.0f, 0.5f * h / 100.0f);

				if (!useIndices)
				{
					// Create tile with vertices only
					mesh->AddVertex(1.0f, 1.0f, -spriteSize.x - x, -spriteSize.y + y, 0.0f, 0 + verticeOff, 0);
					mesh->AddVertex(0.0f, 0.0f, spriteSize.x - x, spriteSize.y + y, 0.0f, 1 + verticeOff, 0);
					mesh->AddVertex(0.0f, 1.0f, spriteSize.x - x, -spriteSize.y + y, 0.0f, 2 + verticeOff, 0);

					mesh->AddVertex(0.0f, 0.0f, spriteSize.x - x, spriteSize.y + y, 0.0f, 3 + verticeOff, 0);
					mesh->AddVertex(1.0f, 1.0f, -spriteSize.x - x, -spriteSize.y + y, 0.0f, 4 + verticeOff, 0);
					mesh->AddVertex(1.0f, 0.0f, -spriteSize.x - x, spriteSize.y + y, 0.0f, 5 + verticeOff, 0);

					subMesh->vertice_count += 6;
				}
				else
				{
					// Create tile with vertices and indices

					mesh->AddVertex(1.0f, 1.0f, -spriteSize.x - x, -spriteSize.y + y, 0.0f, 0 + verticeOff, 0);
					mesh->AddVertex(0.0f, 1.0f, spriteSize.x - x, -spriteSize.y + y, 0.0f, 1 + verticeOff, 0);
					mesh->AddVertex(0.0f, 0.0f, spriteSize.x - x, spriteSize.y + y, 0.0f, 2 + verticeOff, 0);
					mesh->AddVertex(1.0f, 0.0f, -spriteSize.x - x, spriteSize.y + y, 0.0f, 3 + verticeOff, 0);

					subMesh->indices[0 + indiceOff] = 0 + verticeOff;
					subMesh->indices[1 + indiceOff] = 2 + verticeOff;
					subMesh->indices[2 + indiceOff] = 1 + verticeOff;
					subMesh->indices[3 + indiceOff] = 2 + verticeOff;
					subMesh->indices[4 + indiceOff] = 0 + verticeOff;
					subMesh->indices[5 + indiceOff] = 3 + verticeOff;
					subMesh->index_count += 6;
					subMesh->vertice_count += 4;
				}
			}
		}
	}
}

void Tilemap::SetOrderInLayer(int orderInLayer)
{
	this->m_orderInLayer = orderInLayer;
	Graphics::SetDrawOrderListAsDirty();
}

void Tilemap::SetColor(const Color& color)
{
	this->color = color;
	for (int x = 0; x < chunkCount; x++)
	{
		for (int y = 0; y < chunkCount; y++)
		{
			TilemapChunk* chunk = chunks[(size_t)x + (size_t)y * chunkCount];

			// Delete chunk meshes
			int meshSize = (int)chunk->meshes.size();
			for (int i = 0; i < meshSize; i++)
			{
				chunk->meshes[i]->unifiedColor = this->color;
			}
		}
	}
}

void Tilemap::CreateChunksMeshes()
{
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
			TilemapChunk* chunk = chunks[(size_t)x + (size_t)y * chunkCount];

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
				MeshData* mesh = new MeshData(verticesPerTile * chunkSize * chunkSize, indicesPerTile * chunkSize * chunkSize, false, false, true);
				mesh->m_subMeshes[0]->index_count = 0;
				mesh->m_subMeshes[0]->vertice_count = 0;
				mesh->m_hasIndices = useIndices;
				mesh->unifiedColor = color;
				chunk->meshes.push_back(mesh);
			}
		}
	}
}

void Tilemap::CreateRenderCommands(RenderBatch& renderBatch)
{
	/*if (!material || !texture)
		return;

	RenderCommand command = RenderCommand();
	command.material = material;
	command.drawable = this;
	command.subMesh = nullptr;
	command.transform = GetTransform();
	command.isEnabled = IsEnabled() && GetGameObject()->IsLocalActive();

	renderBatch.spriteCommands.push_back(command);
	renderBatch.spriteCommandIndex++;*/
}

void Tilemap::OnDisabled()
{
	Graphics::s_isRenderingBatchDirty = true;
}

void Tilemap::OnEnabled()
{
	Graphics::s_isRenderingBatchDirty = true;
}

void Tilemap::DrawCommand(const RenderCommand& renderCommand)
{
	if (dirtyMeshes)
	{
		dirtyMeshes = false;

		CreateChunksMeshes();
		FillChunks();
	}

	DrawChunks();
}

void Tilemap::DrawChunks()
{
	if (Graphics::usedCamera)
	{
		Vector3 cameraPos = Graphics::usedCamera->GetTransform()->GetPosition();

		float xArea = Graphics::usedCamera->GetProjectionSize() * Graphics::usedCamera->GetAspectRatio() + chunkSize;
		float yArea = Graphics::usedCamera->GetProjectionSize() + chunkSize;

		float xChunkPosition;
		float yChunkPosition;

		auto transform = GetTransform();

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
						TilemapChunk* chunk = chunks[(size_t)x + (size_t)y * chunkCount];
						// Draw each texture
						for (int textureI = 0; textureI < textureSize; textureI++)
						{
							//MeshManager::DrawMesh(transform->GetPosition(), transform->GetEulerAngles(), transform->GetLocalScale(), textures[(size_t)textureI + 1], chunk->meshes[textureI], false, true, false);
						}
					}
				}
			}
		}
	}
}

int Tilemap::GetTextureIndex(Texture* texture) const
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

void Tilemap::AddTexture(Texture* texture)
{
	if (GetTextureIndex(texture) == -1)
	{
		textures.push_back(texture);
		textureSize = (int)textures.size() - 1;
	}
}

void Tilemap::RemoveTexture(Texture* texture)
{
	int index = GetTextureIndex(texture);
	if (index != -1)
	{
		textures.erase(textures.begin() + index);
		textureSize = (int)textures.size() - 1;
	}
}

#endif // ENABLE_EXPERIMENTAL_FEATURES