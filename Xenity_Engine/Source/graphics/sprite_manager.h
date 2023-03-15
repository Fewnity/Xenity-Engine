#pragma once

class Material;
class Texture;
class Vector3;
class Transform;
#include <glm/glm.hpp>

class SpriteManager
{
public:
	static void Init();
	static void RenderSprite(Transform transform, float w, float h, const Texture* texture, Material* material);
	static void RenderSprite(Vector3 position, float w, float h, Vector3 scale, Vector3 rotation, const Texture* texture, Material* material);
	static void RenderSprite(float x, float y, float z, float w, float h, float scaleX, float scaleY, float xAngle, float yAngle, float zAngle, const Texture* texture, Material* material);
	static void RenderSprite(glm::mat4 transformationMatrix, float w, float h, const Texture* texture, Material* material);
	static void CreateSpriteBuffer();
private:
	static unsigned int spriteVAO, spriteVBO;
	static void UpdateMaterial(Material* material, glm::mat4 transformationMatrix);
};

