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
void SpriteManager::RenderSprite(Transform transform, float w, float h, Texture* texture, Shader* s) {
	SpriteManager::RenderSprite(transform.GetPosition(),
		w,
		h,
		transform.GetScale(),
		transform.GetLocalRotation(),
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
void SpriteManager::RenderSprite(Vector3 position, float w, float h, Vector3 scale, Vector3 rotation, Texture* texture, Shader* s) {
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
void SpriteManager::RenderSprite(float x, float y, float z, float w, float h, float scaleX, float scaleY, float xAngle, float yAngle, float zAngle, Texture* texture, Shader* s)
{
	if (texture == nullptr || s == nullptr)
		return;

	//y = Window::GetHeight() - y - h;

	s->Use();
	s->SetShaderCameraPosition();
	//s->SetShaderProjection3D();
	s->SetShaderProjection2D();
	s->SetShaderModel(Vector3(x, -y, z), Vector3(xAngle, yAngle, zAngle), Vector3(scaleX, scaleY, 1));

	/*s->SetShaderPosition(Graphics::usedCamera->gameObject->transform.GetPosition());
	s->SetShaderRotation(Vector3(xAngle, yAngle, zAngle));
	s->SetShaderScale(Vector3(scaleX, scaleY, 1));*/

	//glUniform3f(glGetUniformLocation(s->GetProgramId(), "spriteColor"), 1, 1, 1);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(spriteVAO);

	y = Window::GetHeight() - h;

	float xpos = x;
	float ypos = y;

	// update VBO for each character
	float vertices[6][5] = {
		{ xpos,     ypos + h,z,   0.0f, 0.0f },
		{ xpos,     ypos,z,       0.0f, 1.0f },
		{ xpos + w, ypos, z,      1.0f, 1.0f },

		{ xpos,     ypos + h,z,   0.0f, 0.0f },
		{ xpos + w, ypos, z,      1.0f, 1.0f },
		{ xpos + w, ypos + h,z,   1.0f, 0.0f }
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 5, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
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