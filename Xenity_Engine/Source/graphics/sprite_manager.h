#pragma once

class Shader;
class Texture;
class Vector3;
class Transform;

class SpriteManager
{
public:
	static void Init();
	static void RenderSprite(Transform transform, float w, float h, const Texture* texture, Shader* s);
	static void RenderSprite(Vector3 position, float w, float h, Vector3 scale, Vector3 rotation, const Texture* texture, Shader* s);
	static void RenderSprite(float x, float y, float z, float w, float h, float scaleX, float scaleY, float xAngle, float yAngle, float zAngle, const Texture* texture, Shader* s);
	static void CreateSpriteBuffer();

private:
	static unsigned int spriteVAO, spriteVBO;
};

