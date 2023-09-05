#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include "../iDrawable.h"
#include <vector>
#include "../color/color.h"
#include "../../engine.h"

class Texture;
class MeshData;

class API Tilemap : public IDrawable
{
public:
	Tilemap();
	//void SetReflection();
	std::unordered_map<std::string, ReflectionEntry> GetReflection();

	class Tile
	{
	public:
		Texture* texture = nullptr;
		int textureId = 0;
	};

	int GetDrawPriority() const;

	/**
	* @brief Setup the Tilemap before usage (chunkSize to default)
	*
	* @param width Tilemap width
	* @param height Tilemap height
	*/
	void Setup(int width, int height);

	/**
	* @brief Setup the Tilemap before usage
	*
	* @param width Tilemap width
	* @param height Tilemap height
	* @param chunkSize Size of a chunk
	*/
	void Setup(int width, int height, int chunkSize);

	/**
	* @brief Return tile at position (nullptr if out of bound)
	*
	* @param x Tile X position
	* @param y Tile Y position
	* @return Tilemap::Tile*
	*/
	Tile *GetTile(int x, int y);

	/**
	* @brief Set tile texture (slower)
	*
	* @param x Tile X position
	* @param y Tile Y position
	* @param texture Texture to use or nullptr to clear tile (Texture needs to be added before, see Tilemap::AddTexture)
	*/
	void SetTile(int x, int y, Texture *textureId);

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
	void AddTexture(Texture *texture);

	/**
	* @brief Remove a texture from the Tilemap's texture list
	*
	* @param texture Texture to remove
	*/
	void RemoveTexture(Texture *texture);

	/**
	* Get tile map width (column)
	*/
	int GetWidth()
	{
		return width;
	}

	/**
	* Get tile map height (row)
	*/
	int GetHeight()
	{
		return height;
	}


	void SetOrderInLayer(int orderInLayer)
	{
		this->orderInLayer = orderInLayer;
		Engine::drawOrderListDirty = true;
	}


	int GetOrderInLayer() const
	{
		return orderInLayer;
	}

	/**
	* Set tilemap global color
	*/
	void SetColor(Color color);

private:
	class TilemapChunk
	{
	public:
		std::vector<MeshData *> meshes;
	};
	int chunkSize = 0;
	bool dirtyMeshes = false;

	int orderInLayer = 0;
	void Draw();

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
	int GetTextureIndex(Texture *texture);

	std::vector<Texture *> textures;
	std::vector<TilemapChunk *> chunks;
	int width = 0;
	int height = 0;
	Tile *tiles = nullptr;
	Color color = Color();
	bool needUpdateVertices = true;
	bool useIndices = false;

	int textureSize;
	int chunkCount;
};
