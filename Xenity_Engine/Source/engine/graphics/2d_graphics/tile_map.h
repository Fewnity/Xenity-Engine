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

	class Tile
	{
	public:
		Texture* texture = nullptr;
		int textureId = 0;
	};

	int GetDrawPriority() const;
	void Setup(int width, int height);
	void Setup(int width, int height, int chunkSize);
	Tile *GetTile(int x, int y);
	void SetTile(int x, int y, Texture *textureId);
	void SetTile(int x, int y, int textureId);
	void AddTexture(Texture *texture);
	void RemoveTexture(Texture *texture);

	int GetWidth()
	{
		return width;
	}

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

	void FillChunks();
	void CreateChunksMeshes();
	void DrawChunks();
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
