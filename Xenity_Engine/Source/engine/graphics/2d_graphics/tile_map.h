// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <vector>

#include <engine/api.h>
#include <engine/graphics/iDrawable.h>
#include <engine/graphics/color/color.h>
#include <engine/engine.h>
#include <engine/game_elements/gameplay_manager.h>

class Texture;
class MeshData;

class API Tilemap : public IDrawable
{
public:
	Tilemap();
	~Tilemap();

	class Tile
	{
	public:
		Texture* texture = nullptr;
		int textureId = 0;
	};

	/**
	* @brief Setup the Tilemap before usage (chunkSize to default)
	*
	* @param width Tilemap width
	* @param height Tilemap height
	*/
	void Setup(int _width, int _height);

	/**
	* @brief Setup the Tilemap before usage
	*
	* @param width Tilemap width
	* @param height Tilemap height
	* @param chunkSize Size of a chunk
	*/
	void Setup(int _width, int _height, int _chunkSize);

	/**
	* @brief Return tile at position (nullptr if out of bound)
	*
	* @param x Tile X position
	* @param y Tile Y position
	* @return Tilemap::Tile*
	*/
	Tile* GetTile(int x, int y) const;

	/**
	* @brief Set tile texture (slower)
	*
	* @param x Tile X position
	* @param y Tile Y position
	* @param texture Texture to use or nullptr to clear tile (Texture needs to be added before, see Tilemap::AddTexture)
	*/
	void SetTile(int x, int y, Texture* textureId);

	/**
	* @brief Set tile texture
	*
	* @param x Tile X position
	* @param y Tile Y position
	* @param textureId Texture id to use or 0 to clear tile (Texture needs to be added before, see Tilemap::AddTexture)
	*/
	void SetTile(int x, int y, int textureId);

	/**
	* @brief Add a texture to the Tilemap's texture list
	*
	* @param texture Texture to add
	*/
	void AddTexture(Texture* texture);

	/**
	* @brief Remove a texture from the Tilemap's texture list
	*
	* @param texture Texture to remove
	*/
	void RemoveTexture(Texture* texture);

	/**
	* Get tile map width (column)
	*/
	inline int GetWidth() const
	{
		return width;
	}

	/**
	* Get tile map height (row)
	*/
	inline int GetHeight() const
	{
		return height;
	}

	void SetOrderInLayer(int orderInLayer);

	inline int GetOrderInLayer() const
	{
		return m_orderInLayer;
	}

	/**
	* Set tilemap global color
	*/
	void SetColor(const Color& color);

protected:
	ReflectiveData GetReflectiveData() override;
	void OnDisabled() override;
	void OnEnabled() override;
	void CreateRenderCommands(RenderBatch& renderBatch) override;

	class TilemapChunk
	{
	public:
		std::vector<MeshData*> meshes;
	};
	int chunkSize = 0;
	bool dirtyMeshes = false;

	int m_orderInLayer = 0;
	void DrawCommand(const RenderCommand& renderCommand) override;

	/**
	* @brief Fill all chunks meshes
	*
	*/
	void FillChunks();

	/**
	* @brief Delete and create new meshes for each chunk
	*
	*/
	void CreateChunksMeshes();

	/**
	* @brief Draw visible chunks
	*
	*/
	void DrawChunks();

	/**
	* @brief Get texture index
	*
	* @param texture
	* @return int Texture index
	*/
	int GetTextureIndex(Texture* texture) const;

	std::vector<Texture*> textures;
	std::vector<TilemapChunk*> chunks;
	int width = 0;
	int height = 0;
	Tile* tiles = nullptr;
	Color color = Color();
	bool needUpdateVertices = true;
	bool useIndices = false;

	int textureSize;
	int chunkCount;
};
