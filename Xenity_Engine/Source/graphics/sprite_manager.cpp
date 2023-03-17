#include "sprite_manager.h"
#include <glad/glad.h>
#include <iostream>
#include "texture.h"
#include "material.h"
#include "../ui/window.h"
#include "../transform.h"
#include "graphics.h"
#include "../debug/debug.h"

unsigned int SpriteManager::spriteVAO, SpriteManager::spriteVBO;

#pragma region Drawing

/// <summary>
/// Draw a sprite
/// </summary>
/// <param name="transform">Sprite's transform</param>
/// <param name="w">Sprite's width</param>
/// <param name="h">Sprite's heigh</param>
/// <param name="texture">Sprite's texture</param>
/// <param name="s">Sprite's shader</param>
void SpriteManager::RenderSprite(Transform transform, float w, float h, const Texture* texture, Material* material) {
	SpriteManager::RenderSprite(transform.GetPosition(),
		w,
		h,
		transform.GetScale(),
		transform.GetRotation(),
		texture, material);
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
void SpriteManager::RenderSprite(Vector3 position, float w, float h, Vector3 scale, Vector3 rotation, const Texture* texture, Material* material) {
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
		texture, material);
}

/// <summary>
/// Draw a sprite (Raw values)
/// </summary>
/// <param name="x">X position</param>
/// <param name="y">Y position</param>
/// <param name="z">Z position</param>
/// <param name="w">Width</param>
/// <param name="h">Heigh</param>
/// <param name="scaleX">X scale</param>
/// <param name="scaleY">Y scale </param>
/// <param name="xAngle">X angle</param>
/// <param name="yAngle">Y angle</param>
/// <param name="zAngle">Z angle</param>
/// <param name="texture">Sprite's texture</param>
/// <param name="s">Sprite's shader</param>
void SpriteManager::RenderSprite(float x, float y, float z, float w, float h, float scaleX, float scaleY, float xAngle, float yAngle, float zAngle, const Texture* texture, Material* material)
{
	if (texture == nullptr || material == nullptr)
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

	//UpdateMaterial(material);
	//material->shader->SetShaderModel(Vector3(x, y, z), Vector3(xAngle, yAngle, zAngle), Vector3(scaleX / aspect * 1.7777f, scaleY / aspect * 1.7777f, 1));

	material->Use();
	material->shader->SetShaderCameraPosition2D();
	material->shader->SetShaderProjection2D();
	material->shader->SetShaderModel(Vector3(x, y, z), Vector3(xAngle, yAngle, zAngle), Vector3(scaleX / aspect * 1.7777f * w, scaleY / aspect * 1.7777f * h, 1));
	//return;

	//glUniform3f(glGetUniformLocation(s->GetProgramId(), "spriteColor"), 1, 1, 1);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(spriteVAO);

	/*float h2 = h / 2.0f;
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
	};*/
	// render glyph texture over quad
	glBindTexture(GL_TEXTURE_2D, texture->GetTextureId());
	// update content of VBO memory
	glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// render quad
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}



//void SpriteManager::RenderSprite(glm::mat4 transformationMatrix, float w, float h, const Texture* texture, Material* material)
void SpriteManager::RenderSprite(glm::mat4 transformationMatrix, const Texture* texture, Material* material)
{

	float unitCoef = 100.0f / texture->GetPixelPerUnit();
	float w = texture->GetWidth() * unitCoef;
	float h = texture->GetHeight() * unitCoef;
	w /= 720;
	h /= 720;

	if (texture == nullptr || material == nullptr)
		return;

	float sizeFixer = 100 / 720.0f;

	//Scale
	transformationMatrix[0].x *= w;
	transformationMatrix[0].y *= h;
	transformationMatrix[1].x *= w;
	transformationMatrix[1].y *= h;
	//Move
	transformationMatrix[3].x *= sizeFixer;
	transformationMatrix[3].y *= sizeFixer;
	UpdateMaterial(material, transformationMatrix);

	// update VBO for each character
	glDisable(GL_DEPTH_TEST);
	//glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	// TODO : Optimisation : Do not bind texture if the texture is already in use
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->GetTextureId());

	glBindVertexArray(spriteVAO);

	// render quad
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

#pragma endregion

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
	//Vertices attrib
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	//Uv attrib
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	float x = -0.5f;
	float vertices[6][4] = {
		{ x,     x + 1,   0.0f, 0.0f },
		{ x,     x,        0.0f, 1.0f },
		{ x + 1, x,       1.0f, 1.0f },

		{ x,     x + 1,    0.0f, 0.0f },
		{ x + 1, x,      1.0f, 1.0f },
		{ x + 1, x + 1,   1.0f, 0.0f }
	};
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void SpriteManager::UpdateMaterial(Material* material, glm::mat4 transformationMatrix)
{
	if (material != nullptr && material->shader != nullptr)
	{
		material->Use();
		bool noNeedUpdate = material->updated;
		if (!noNeedUpdate)
		{
			material->Update();
			material->shader->SetShaderCameraPosition2D();
			material->shader->SetShaderProjection2D();
		}
		material->shader->SetShaderModel(transformationMatrix);
	}
}

/// <summary>
/// Init sprite manager
/// </summary>
void SpriteManager::Init() {
	CreateSpriteBuffer();

	Debug::Print("---- Sprite System initiated ----");
}