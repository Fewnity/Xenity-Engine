#include "sprite_manager.h"
#include <glad/glad.h>
#include <iostream>
#include "texture.h"
#include "material.h"
#include "../ui/window.h"
#include "../transform.h"
#include "graphics.h"
#include "../debug/debug.h"
#include "../tools/profiler_benchmark.h"
#include "../time/time.h"
#include "../debug/performance.h"

unsigned int SpriteManager::spriteVAO, SpriteManager::spriteVAOSmall, SpriteManager::spriteVBO, SpriteManager::spriteVBOSmall;
ProfilerBenchmark* spriteBenchmark = new ProfilerBenchmark("Sprites");
float  SpriteManager::vertices2[6 * 600 * 600][4];
const Texture* SpriteManager::currentTexture = nullptr;
unsigned int SpriteManager::lineVAO, SpriteManager::lineVBO;

void UpdateMaterial(Material* material, glm::mat4* transformationMatrix);

#pragma region Drawing

/// <summary>
/// Draw a sprite
/// </summary>
/// <param name="transform">Sprite's transform</param>
/// <param name="w">Sprite's width</param>
/// <param name="h">Sprite's heigh</param>
/// <param name="texture">Sprite's texture</param>
/// <param name="s">Sprite's shader</param>
void SpriteManager::RenderSprite(Transform transform, float w, float h, Vector4 color, const Texture* texture, Material* material) {
	SpriteManager::RenderSprite(transform.GetPosition(),
		w,
		h,
		transform.GetScale(),
		transform.GetRotation(),
		color, texture, material);
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
void SpriteManager::RenderSprite(Vector3 position, float w, float h, Vector3 scale, Vector3 rotation, Vector4 color, const Texture* texture, Material* material) {
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
		color, texture, material);
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
void SpriteManager::RenderSprite(float x, float y, float z, float w, float h, float scaleX, float scaleY, float xAngle, float yAngle, float zAngle, Vector4 color, const Texture* texture, Material* material)
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
	material->shader->SetShaderProjection();
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
	Performance::AddDrawCall();

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void SpriteManager::Render2DLine(Vector2 start, Vector2 end, float width, Vector4& color, Material* material)
{
	glm::mat4 t = glm::mat4(1);
	UpdateMaterial(material, &t);

	float sizeFixer = 0.1f;

	Vector2 dir = (end - start).normalize();

	start *= sizeFixer;
	end *= sizeFixer;
	width *= sizeFixer;

	float fixedXWidth = width / 2.0f * dir.y;
	float fixedYWidth = width / 2.0f * dir.x;
	
	//Create line's vertices and uv
	float vertices[6][4] = {
		{ end.x - fixedXWidth,     end.y + fixedYWidth,   0.0f, 0.0f },
		{ start.x - fixedXWidth ,    start.y + fixedYWidth,        0.0f, 1.0f },
		{ start.x + fixedXWidth, start.y - fixedYWidth,       1.0f, 1.0f },

		{ end.x - fixedXWidth,     end.y + fixedYWidth,    0.0f, 0.0f },
		{ start.x + fixedXWidth, start.y - fixedYWidth,      1.0f, 1.0f },
		{ end.x + fixedXWidth, end.y - fixedYWidth,   1.0f, 0.0f }
	};

	if (currentTexture != AssetManager::defaultTexture)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, AssetManager::defaultTexture->GetTextureId());
	}

	glBindVertexArray(lineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// render quad
	glDrawArrays(GL_TRIANGLES, 0, 6);
	Performance::AddDrawCall();
}

//void SpriteManager::RenderSprite(glm::mat4 transformationMatrix, float w, float h, const Texture* texture, Material* material)
void SpriteManager::RenderSprite(glm::mat4 transformationMatrix, Vector4& color, const Texture* texture, Material* material)
{
	if (texture == nullptr || material == nullptr)
		return;

	float diviser = 1000;

	float unitCoef = 100.0f / texture->GetPixelPerUnit() / diviser;
	float w = texture->GetWidth() * unitCoef;
	float h = texture->GetHeight() * unitCoef;


	float sizeFixer = 100 / diviser;

	material->shader->SetShaderAttribut("color", color);

	//Scale
	transformationMatrix[0].x *= w;
	transformationMatrix[0].y *= w;
	transformationMatrix[1].x *= h;
	transformationMatrix[1].y *= h;

	//Move
	transformationMatrix[3].x *= sizeFixer;
	transformationMatrix[3].y *= sizeFixer;

	UpdateMaterial(material, &transformationMatrix);

	if (currentTexture != texture)
	{
		currentTexture = texture;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureId());
	}

	glBindVertexArray(spriteVAOSmall);
	glBindBuffer(GL_ARRAY_BUFFER, spriteVBOSmall);

	// render quad
	glDrawArrays(GL_TRIANGLES, 0, 6);
	Performance::AddDrawCall();
}

#pragma endregion

/// <summary>
/// Create a buffer for sprites
/// </summary>
void SpriteManager::CreateSpriteBuffer()
{
	glGenVertexArrays(1, &spriteVAOSmall);
	glGenBuffers(1, &spriteVBOSmall);

	glBindVertexArray(spriteVAOSmall);
	glBindBuffer(GL_ARRAY_BUFFER, spriteVBOSmall);
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

	//////////////////////////////////////
	glGenVertexArrays(1, &spriteVAO);
	glGenBuffers(1, &spriteVBO);
	glBindVertexArray(spriteVAO);
	glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 600 * 600 * 4, NULL, GL_DYNAMIC_DRAW);
	//Vertices attrib
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	//Uv attrib
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//////////////////////////////////////
	glGenVertexArrays(1, &lineVAO);
	glGenBuffers(1, &lineVBO);
	glBindVertexArray(lineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	//Vertices attrib
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	//Uv attrib
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void SpriteManager::AddToBatch(const Texture* texture, glm::mat4& transformationMatrix, int index, float x, float y, float scaleX, float scaleY)
{

	/*float diviser = 1000 * Graphics::usedCamera->GetProjectionSize() / 5.0f;

	float unitCoef = 100.0f / texture->GetPixelPerUnit();
	float w = texture->GetWidth() * unitCoef;
	float h = texture->GetHeight() * unitCoef;
	w /= diviser;
	h /= diviser;

	if (texture == nullptr)
		return;

	float sizeFixer = 100 / diviser;

	float x = transformationMatrix[3].x * sizeFixer;
	float y = transformationMatrix[3].y * sizeFixer;

	float xpos1 = x;
	float xpos2 = x + w;
	float ypos1 = y;
	float ypos2 = y + h;

	xpos1 -= w / 2.0f;
	xpos2 -= w / 2.0f;
	ypos1 -= h / 2.0f;
	ypos2 -= h / 2.0f;

	Vector2 c1 = Vector2(xpos1, ypos2);
	Vector2 c2 = Vector2(xpos1, ypos1);
	Vector2 c3 = Vector2(xpos2, ypos1);
	Vector2 c4 = Vector2(xpos2, ypos2);
	float cx = (c1.x + c2.x + c3.x + c4.x) / 4.0f;
	float cy = (c1.y + c2.y + c3.y + c4.y) / 4.0f;

	float angle = 45;
	//tim += Time::GetDeltaTime() / 100.0f;
	float angleRad = angle * M_PI / 180.0f;

	float angleCos = cos(angleRad);
	float angleSin = sin(angleRad);
	Vector2 c1n = Vector2(cx + (c1.x - cx) * angleCos - (c1.y - cy) * angleSin, cy + (c1.x - cx) * angleSin + (c1.y - cy) * angleCos);
	Vector2 c2n = Vector2(cx + (c2.x - cx) * angleCos - (c2.y - cy) * angleSin, cy + (c2.x - cx) * angleSin + (c2.y - cy) * angleCos);
	Vector2 c3n = Vector2(cx + (c3.x - cx) * angleCos - (c3.y - cy) * angleSin, cy + (c3.x - cx) * angleSin + (c3.y - cy) * angleCos);
	Vector2 c4n = Vector2(cx + (c4.x - cx) * angleCos - (c4.y - cy) * angleSin, cy + (c4.x - cx) * angleSin + (c4.y - cy) * angleCos);

	int realIndex = index * 6;
	vertices2[realIndex][0] = c1n.x;
	vertices2[realIndex][1] = c1n.y;
	vertices2[realIndex][2] = 0.0f;
	vertices2[realIndex][3] = 0.0f;

	vertices2[realIndex + 1][0] = c2n.x;
	vertices2[realIndex + 1][1] = c2n.y;
	vertices2[realIndex + 1][2] = 0.0f;
	vertices2[realIndex + 1][3] = 1.0f;

	vertices2[realIndex + 2][0] = c3n.x;
	vertices2[realIndex + 2][1] = c3n.y;
	vertices2[realIndex + 2][2] = 1.0f;
	vertices2[realIndex + 2][3] = 1.0f;

	vertices2[realIndex + 3][0] = c1n.x;
	vertices2[realIndex + 3][1] = c1n.y;
	vertices2[realIndex + 3][2] = 0.0f;
	vertices2[realIndex + 3][3] = 0.0f;

	vertices2[realIndex + 4][0] = c3n.x;
	vertices2[realIndex + 4][1] = c3n.y;
	vertices2[realIndex + 4][2] = 1;
	vertices2[realIndex + 4][3] = 1.0f;

	vertices2[realIndex + 5][0] = c4n.x;
	vertices2[realIndex + 5][1] = c4n.y;
	vertices2[realIndex + 5][2] = 1.0f;
	vertices2[realIndex + 5][3] = 0.0f;*/

	float diviser = 1000 * Graphics::usedCamera->GetProjectionSize() / 5.0f;
	float sizeFixer = 100 / diviser;

	float unitCoef = 100.0f / texture->GetPixelPerUnit();
	float w = texture->GetWidth() * unitCoef;
	float h = texture->GetHeight() * unitCoef;
	//w /= diviser;
	//h /= diviser;

	if (texture == nullptr)
		return;

	x *= 100;
	y *= 100;
	//float x = 100*index;
	//float y = 0;

	float xpos1 = x;
	float xpos2 = x + w;
	float ypos1 = y;
	float ypos2 = y + h;

	xpos1 -= w / 2.0f;
	xpos2 -= w / 2.0f;
	ypos1 -= h / 2.0f;
	ypos2 -= h / 2.0f;

	Vector2 c1 = Vector2(xpos1, ypos2);
	Vector2 c2 = Vector2(xpos1, ypos1);
	Vector2 c3 = Vector2(xpos2, ypos1);
	Vector2 c4 = Vector2(xpos2, ypos2);
	float cx = (c1.x + c2.x + c3.x + c4.x) / 4.0f;
	float cy = (c1.y + c2.y + c3.y + c4.y) / 4.0f;

	float angle = 45;
	//tim += Time::GetDeltaTime() / 100.0f;
	float angleRad = angle * M_PI / 180.0f;

	float angleCos = cos(angleRad);
	float angleSin = sin(angleRad);
	Vector2 c1n = Vector2(cx + (c1.x - cx) * angleCos - (c1.y - cy) * angleSin, cy + (c1.x - cx) * angleSin + (c1.y - cy) * angleCos);
	Vector2 c2n = Vector2(cx + (c2.x - cx) * angleCos - (c2.y - cy) * angleSin, cy + (c2.x - cx) * angleSin + (c2.y - cy) * angleCos);
	Vector2 c3n = Vector2(cx + (c3.x - cx) * angleCos - (c3.y - cy) * angleSin, cy + (c3.x - cx) * angleSin + (c3.y - cy) * angleCos);
	Vector2 c4n = Vector2(cx + (c4.x - cx) * angleCos - (c4.y - cy) * angleSin, cy + (c4.x - cx) * angleSin + (c4.y - cy) * angleCos);

	c1n /= diviser;
	c2n /= diviser;
	c3n /= diviser;
	c4n /= diviser;

	int realIndex = index * 6;
	vertices2[realIndex][0] = c1n.x;
	vertices2[realIndex][1] = c1n.y;
	vertices2[realIndex][2] = 0.0f;
	vertices2[realIndex][3] = 0.0f;

	vertices2[realIndex + 1][0] = c2n.x;
	vertices2[realIndex + 1][1] = c2n.y;
	vertices2[realIndex + 1][2] = 0.0f;
	vertices2[realIndex + 1][3] = 1.0f;

	vertices2[realIndex + 2][0] = c3n.x;
	vertices2[realIndex + 2][1] = c3n.y;
	vertices2[realIndex + 2][2] = 1.0f;
	vertices2[realIndex + 2][3] = 1.0f;

	vertices2[realIndex + 3][0] = c1n.x;
	vertices2[realIndex + 3][1] = c1n.y;
	vertices2[realIndex + 3][2] = 0.0f;
	vertices2[realIndex + 3][3] = 0.0f;

	vertices2[realIndex + 4][0] = c3n.x;
	vertices2[realIndex + 4][1] = c3n.y;
	vertices2[realIndex + 4][2] = 1;
	vertices2[realIndex + 4][3] = 1.0f;

	vertices2[realIndex + 5][0] = c4n.x;
	vertices2[realIndex + 5][1] = c4n.y;
	vertices2[realIndex + 5][2] = 1.0f;
	vertices2[realIndex + 5][3] = 0.0f;

	/*float vertices[6][4] = {
		{ c1n.x,     c1n.y,   0.0f, 0.0f },
		{ c2n.x,     c2n.y,        0.0f, 1.0f },
		{ c3n.x, c3n.y,       1.0f, 1.0f },

		{ c1n.x,     c1n.y,    0.0f, 0.0f },
		{ c3n.x, c3n.y,      1.0f, 1.0f },
		{ c4n.x, c4n.y ,   1.0f, 0.0f }
	};*/
}

void SpriteManager::AddToBatch(int index, Vector2 vertices[4])
{
	float diviser = 1000 * Graphics::usedCamera->GetProjectionSize() / 5.0f;
	//float diviser = 500;

	int realIndex = index * 6;

	float* addr = vertices2[realIndex];

	addr[0] = vertices[0].x / diviser;
	addr[1] = vertices[0].y / diviser;
	addr[2] = 0.0f;
	addr[3] = 0.0f;

	addr = vertices2[realIndex + 1];

	addr[0] = vertices[1].x / diviser;
	addr[1] = vertices[1].y / diviser;
	addr[2] = 0.0f;
	addr[3] = 1.0f;

	addr = vertices2[realIndex + 2];

	addr[0] = vertices[2].x / diviser;
	addr[1] = vertices[2].y / diviser;
	addr[2] = 1.0f;
	addr[3] = 1.0f;

	addr = vertices2[realIndex + 3];

	addr[0] = vertices[0].x / diviser;
	addr[1] = vertices[0].y / diviser;
	addr[2] = 0.0f;
	addr[3] = 0.0f;

	addr = vertices2[realIndex + 4];

	addr[0] = vertices[2].x / diviser;
	addr[1] = vertices[2].y / diviser;
	addr[2] = 1;
	addr[3] = 1.0f;

	addr = vertices2[realIndex + 5];

	addr[0] = vertices[3].x / diviser;
	addr[1] = vertices[3].y / diviser;
	addr[2] = 1.0f;
	addr[3] = 0.0f;
}

void SpriteManager::DrawBatch(const Texture* texture, Vector4& color, Material* material)
{
	glm::mat4 t = glm::mat4(1);
	UpdateMaterial(material, &t);

	material->shader->SetShaderAttribut("color", color);

	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->GetTextureId());

	glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices2), vertices2);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	// render quad
	glBindVertexArray(spriteVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6 * 600 * 600);
	Performance::AddDrawCall();
	glBindVertexArray(spriteVAOSmall);
}

void SpriteManager::StartDrawing()
{
	currentTexture = nullptr;
	glDisable(GL_DEPTH_TEST);
	//glBindVertexArray(spriteVAO);
}

void UpdateMaterial(Material* material, glm::mat4* transformationMatrix)
{
	if (material != nullptr && material->shader != nullptr)
	{
		material->Use();
		if (!material->updated)
		{
			material->Update();
			material->shader->SetShaderCameraPosition2D();
			material->shader->SetShaderProjection();
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

SpriteBatch::SpriteBatch(Material* mat, const Texture* texture)
{
	material = mat;
	this->texture = texture;

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
}

void SpriteBatch::Draw(Vector4& color)
{
	if (verticesCount != 0) {
		glm::mat4 t = glm::mat4(1);
		UpdateMaterial(material, &t);

		material->shader->SetShaderAttribut("color", color);

		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureId());

		glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, verticesCount * 4 * sizeof(float), vertices);

		glBindVertexArray(spriteVAO);
		glDrawArrays(GL_TRIANGLES, 0, verticesCount);
		Performance::AddDrawCall();
	}

	index = 0;
}

void SpriteBatch::AddVertices(Vector2 verticesToAdd[4])
{
	float diviser = 1000;

	int realIndex = index * 6 * 4;

	float cx0 = verticesToAdd[0].x / diviser;
	float cy0 = verticesToAdd[0].y / diviser;

	float cx2 = verticesToAdd[2].x / diviser;
	float cy2 = verticesToAdd[2].y / diviser;

	vertices[realIndex] = cx0;
	vertices[realIndex+1] = cy0;
	vertices[realIndex+2] = 0.0f;
	vertices[realIndex+3] = 0.0f;

	//
	vertices[realIndex+4] = verticesToAdd[1].x / diviser;
	vertices[realIndex+5] = verticesToAdd[1].y / diviser;
	vertices[realIndex+6] = 0.0f;
	vertices[realIndex+7] = 1.0f;


	vertices[realIndex+8] = cx2;
	vertices[realIndex+9] = cy2;
	vertices[realIndex+10] = 1.0f;
	vertices[realIndex+11] = 1.0f;


	vertices[realIndex+12] = cx0;
	vertices[realIndex+13] = cy0;
	vertices[realIndex+14] = 0.0f;
	vertices[realIndex+15] = 0.0f;


	vertices[realIndex+16] = cx2;
	vertices[realIndex+17] = cy2;
	vertices[realIndex+18] = 1;
	vertices[realIndex+19] = 1.0f;

	//
	vertices[realIndex+20] = verticesToAdd[3].x / diviser;
	vertices[realIndex+21] = verticesToAdd[3].y / diviser;
	vertices[realIndex+22] = 1.0f;
	vertices[realIndex+23] = 0.0f;

	index++;
}

void SpriteBatch::SetBatchSize()
{
	if (verticesCount != oldVerticeCount) {
		if (vertices != nullptr) {
			free(vertices);
		}
		vertices = (float*)malloc(verticesCount * 4 * sizeof(float));

		glBindVertexArray(spriteVAO);
		glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticesCount * 4, NULL, GL_DYNAMIC_DRAW);
		oldVerticeCount = verticesCount;
	}
}
