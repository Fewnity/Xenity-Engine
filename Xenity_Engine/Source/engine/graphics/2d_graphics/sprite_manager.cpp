#include "sprite_manager.h"
#include "../../../xenity.h"
#include "../color/color.h"
#include <glad/glad.h>
#include <iostream>

unsigned int SpriteManager::spriteVAO, SpriteManager::spriteVAOSmall, SpriteManager::spriteVBO, SpriteManager::spriteVBOSmall;
const Texture* SpriteManager::currentTexture = nullptr;
unsigned int SpriteManager::lineVAO, SpriteManager::lineVBO;

void UpdateMaterial(Material* material, glm::mat4* transformationMatrix);

#pragma region Drawing

void SpriteManager::Render2DLine(Vector3 start, Vector3 end, float width, Color color, Material* material)
{
	glm::mat4 t = glm::mat4(1);
	UpdateMaterial(material, &t);
	Vector4 rgbaColor = color.GetRGBA().ToVector4();
	material->shader->SetShaderAttribut("color", rgbaColor);

	float sizeFixer = 0.1f;

	Vector2 dir = (Vector2(end.x, end.y) - Vector2(start.x, start.y)).normalize();

	start *= sizeFixer;
	end *= sizeFixer;
	width *= sizeFixer;

	float fixedXWidth = width / 2.0f * dir.y;
	float fixedYWidth = width / 2.0f * dir.x;

	//Create line's vertices and uv
	float vertices[6][5] = {
		{ end.x - fixedXWidth,     end.y + fixedYWidth, end.z,  0.0f, 0.0f },
		{ start.x - fixedXWidth ,    start.y + fixedYWidth,end.z,        0.0f, 1.0f },
		{ start.x + fixedXWidth, start.y - fixedYWidth,end.z,       1.0f, 1.0f },

		{ end.x - fixedXWidth,     end.y + fixedYWidth,end.z,    0.0f, 0.0f },
		{ start.x + fixedXWidth, start.y - fixedYWidth,end.z,      1.0f, 1.0f },
		{ end.x + fixedXWidth, end.y - fixedYWidth,end.z,   1.0f, 0.0f }
	};

	glDisable(GL_DEPTH_TEST);

	//glEnable(GL_DEPTH_TEST);

	//if (currentTexture != AssetManager::defaultTexture)
	//{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, AssetManager::defaultTexture->GetTextureId());
	//}

	glBindVertexArray(lineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// render quad
	glDrawArrays(GL_TRIANGLES, 0, 6);
	Performance::AddDrawCall();
}

void SpriteManager::RenderSprite(glm::mat4 transformationMatrix, Color color, const Texture* texture, Material* material)
{
	if (texture == nullptr || material == nullptr)
		return;

	float diviser = 1000;

	float unitCoef = 100.0f / texture->GetPixelPerUnit() / diviser;
	float w = texture->GetWidth() * unitCoef;
	float h = texture->GetHeight() * unitCoef;


	float sizeFixer = 100 / diviser;
	Vector4 rgbaColor = color.GetRGBA().ToVector4();
	material->shader->SetShaderAttribut("color", rgbaColor);

	//Scale
	transformationMatrix[0].x *= w;
	transformationMatrix[0].y *= w;
	transformationMatrix[1].x *= h;
	transformationMatrix[1].y *= h;

	if ((transformationMatrix[1].y < 0 && transformationMatrix[0].x < 0) || (transformationMatrix[1].y > 0 && transformationMatrix[0].x > 0))
	{
		glFrontFace(GL_CCW);
	}
	else 
	{
		glFrontFace(GL_CW);
	}

	//Move
	transformationMatrix[3].x *= -sizeFixer;
	transformationMatrix[3].y *= sizeFixer;

	UpdateMaterial(material, &transformationMatrix);
	glDisable(GL_DEPTH_TEST);
	/*glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

	// 
	//if (currentTexture != texture)
	//{
	currentTexture = texture;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->GetTextureId());
	//}

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 5, NULL, GL_DYNAMIC_DRAW);
	//Vertices attrib
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	//Uv attrib
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
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

void SpriteBatch::Draw(Color color)
{
	if (verticesCount != 0) 
	{
		glm::mat4 t = glm::mat4(1);
		UpdateMaterial(material, &t);

		Vector4 rgbaColor = color.GetRGBA().ToVector4();
		material->shader->SetShaderAttribut("color", rgbaColor);

		glDisable(GL_DEPTH_TEST);
		//glEnable(GL_DEPTH_TEST);

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
	vertices[realIndex + 1] = cy0;
	vertices[realIndex + 2] = 0.0f;
	vertices[realIndex + 3] = 0.0f;

	//
	vertices[realIndex + 4] = verticesToAdd[1].x / diviser;
	vertices[realIndex + 5] = verticesToAdd[1].y / diviser;
	vertices[realIndex + 6] = 0.0f;
	vertices[realIndex + 7] = 1.0f;


	vertices[realIndex + 8] = cx2;
	vertices[realIndex + 9] = cy2;
	vertices[realIndex + 10] = 1.0f;
	vertices[realIndex + 11] = 1.0f;


	vertices[realIndex + 12] = cx0;
	vertices[realIndex + 13] = cy0;
	vertices[realIndex + 14] = 0.0f;
	vertices[realIndex + 15] = 0.0f;


	vertices[realIndex + 16] = cx2;
	vertices[realIndex + 17] = cy2;
	vertices[realIndex + 18] = 1;
	vertices[realIndex + 19] = 1.0f;

	//
	vertices[realIndex + 20] = verticesToAdd[3].x / diviser;
	vertices[realIndex + 21] = verticesToAdd[3].y / diviser;
	vertices[realIndex + 22] = 1.0f;
	vertices[realIndex + 23] = 0.0f;

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
