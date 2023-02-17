#include "sprite_manager.h"
#include <glad/glad.h>
#include <iostream>
#include "texture.h"
#include "shader.h"
#include "../ui/window.h"
#include "../transform.h"
#include "graphics.h"
#include "../debug.h"

unsigned int SpriteManager::spriteVAO, SpriteManager::spriteVBO;

/// <summary>
/// Draw a sprite
/// </summary>
/// <param name="transform">Sprite's transform</param>
/// <param name="w">Sprite's width</param>
/// <param name="h">Sprite's heigh</param>
/// <param name="texture">Sprite's texture</param>
/// <param name="s">Sprite's shader</param>
void SpriteManager::RenderSprite(Transform transform, float w, float h, const Texture* texture, Shader* s) {
	SpriteManager::RenderSprite(transform.GetPosition(),
		w,
		h,
		transform.GetScale(),
		transform.GetRotation(),
		texture, s);
}

/// <summary>
/// Draw a sprite
/// </summary>
/// <param name="position">Sprite's position</param>
/// <param name="w">Sprite's width</param>
/// <param name="h">Sprite's heigh</param>
/// <param name="scale">Sprite's scale</param>
/// <param name="rotation">Sprite's rotation</param>
/// <param name="texture">Sprite's texture</param>
/// <param name="s">Sprite's shader</param>
void SpriteManager::RenderSprite(Vector3 position, float w, float h, Vector3 scale, Vector3 rotation, const Texture* texture, Shader* s) {
	SpriteManager::RenderSprite(position.x,
								position.y,
								position.z,
								w,
								h,
								scale.x,
								scale.y,
								rotation.x,
								rotation.y,
								rotation.z,
								texture, s);
}

/// <summary>
/// Draw a sprite (Raw values)
/// </summary>
/// <param name="x">X position</param>
/// <param name="y">Y position</param>
/// <param name="z">Z position</param>
/// <param name="w">Xidth</param>
/// <param name="h">Heigh</param>
/// <param name="scaleX">X scale</param>
/// <param name="scaleY">Y scale </param>
/// <param name="xAngle">X angle</param>
/// <param name="yAngle">Y angle</param>
/// <param name="zAngle">Z angle</param>
/// <param name="texture">Sprite's texture</param>
/// <param name="s">Sprite's shader</param>
void SpriteManager::RenderSprite(float x, float y, float z, float w, float h, float scaleX, float scaleY, float xAngle, float yAngle, float zAngle, const Texture* texture, Shader* s)
{
	if (texture == nullptr || s == nullptr)
		return;

	float aspect = static_cast<float>((Window::GetWidth()) / static_cast<float>(Window::GetHeight()));
	
	x -= Graphics::usedCamera->gameObject->transform.GetPosition().x;
	y += Graphics::usedCamera->gameObject->transform.GetPosition().y;

	x = x / aspect * 1.7777f;
	y = y / aspect * 1.7777f;

	x += Window::GetWidth() / 100.0f / 2.0f;
	y += Window::GetHeight() / 100.0f / 2.0f;

	y *= 100;
	x *= 100;
	y = Window::GetHeight() - y;

	s->Use();
	s->SetShaderCameraPosition2D();
	s->SetShaderProjection2D();
	s->SetShaderModel(Vector3(x, y, z), Vector3(xAngle, yAngle, zAngle), Vector3(scaleX / aspect * 1.7777f, scaleY / aspect * 1.7777f, 1));

	//glUniform3f(glGetUniformLocation(s->GetProgramId(), "spriteColor"), 1, 1, 1);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(spriteVAO);

	float h2 = h / 2.0f;
	float w2 = w / 2.0f;
	float xpos = -w2;
	float ypos = -h2;

	// update VBO for each character
	float vertices[6][4] = {
		{ xpos,     ypos + h,   0.0f, 0.0f },
		{ xpos,     ypos,        0.0f, 1.0f },
		{ xpos + w, ypos,       1.0f, 1.0f },

		{ xpos,     ypos + h,    0.0f, 0.0f },
		{ xpos + w, ypos,      1.0f, 1.0f },
		{ xpos + w, ypos + h,   1.0f, 0.0f }
	};

	// render glyph texture over quad
	glBindTexture(GL_TEXTURE_2D, texture->GetTextureId());
	// update content of VBO memory
	glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// render quad
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/// <summary>
/// Create a buffer for sprites
/// </summary>
void SpriteManager::CreateSpriteBuffer()
{
	glGenVertexArrays(1, &spriteVAO);
	glGenBuffers(1, &spriteVBO);
	glBindVertexArray(spriteVAO);
	glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

/// <summary>
/// Init sprite manager
/// </summary>
void SpriteManager::Init() {
	CreateSpriteBuffer();

	Debug::Print("---- Sprite System initiated ----");
}