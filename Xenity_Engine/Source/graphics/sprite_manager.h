#pragma once

class Material;
class Texture;
class Vector3;
class Vector2;
class Vector4;
class Transform;
#include <glm/glm.hpp>

class SpriteBatch {
public:
	SpriteBatch() = delete;
	SpriteBatch(Material *mat, const Texture* texture);
	void Draw(Vector4& color);
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
	static void RenderSprite(Transform transform, float w, float h, Vector4 color, const Texture* texture, Material* material);
	static void RenderSprite(Vector3 position, float w, float h, Vector3 scale, Vector3 rotation, Vector4 color, const Texture* texture, Material* material);
	static void RenderSprite(float x, float y, float z, float w, float h, float scaleX, float scaleY, float xAngle, float yAngle, float zAngle, Vector4 color, const Texture* texture, Material* material);
	//static void RenderSprite(glm::mat4 transformationMatrix, float w, float h, const Texture* texture, Material* material);
	static void RenderSprite(glm::mat4 transformationMatrix, Vector4 &color, const Texture* texture, Material* material);
	static void CreateSpriteBuffer();
	static void AddToBatch(const Texture* texture, glm::mat4& transformationMatrix, int index, float x, float y, float scaleX, float scaleY);
	static void AddToBatch(int index, Vector2 vertices[4]);
	
	static void DrawBatch(const Texture* texture, Vector4& color, Material* material);
	static void StartDrawing();

private:
	static unsigned int spriteVAO, spriteVBO, spriteVAOSmall, spriteVBOSmall;
	//static void UpdateMaterial(Material* material, glm::mat4 *transformationMatrix);
	static float vertices2[6* 600 * 600][4];
	static const Texture* currentTexture;
};

