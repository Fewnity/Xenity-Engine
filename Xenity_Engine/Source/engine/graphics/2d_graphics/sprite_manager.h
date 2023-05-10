#pragma once

class Material;
class Texture;
class Vector3;
class Vector2;
class Vector4;
class Color;
class Transform;
#include <glm/glm.hpp>

class SpriteBatch {
public:
	SpriteBatch() = delete;
	SpriteBatch(Material *mat, const Texture* texture);
	void Draw(Color color);
	void AddVertices(Vector2 vertices[4]);
	void SetBatchSize();

	unsigned int spriteVAO;
	unsigned int spriteVBO;
	int verticesCount = 0;
	Material* material = nullptr;
	const Texture* texture = nullptr;
	float * vertices = nullptr;
	int oldVerticeCount = 0;
	int index = 0;
};

class SpriteManager
{
public:
	static void Init();
	static void CreateSpriteBuffers();
	static void Render2DLine(Vector3 start, Vector3 end, float width, Color color, Material* material);
	static void RenderSprite(glm::mat4 transformationMatrix, Color color, const Texture* texture, Material* material);

private:
	static unsigned int spriteVAOSmall, spriteVBOSmall;
	static unsigned int lineVAO, lineVBO;
	static const Texture* currentTexture;
};

