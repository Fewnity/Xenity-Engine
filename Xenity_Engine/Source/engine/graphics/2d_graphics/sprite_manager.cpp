#include "sprite_manager.h"
#include "../../../xenity.h"
#include "../color/color.h"

#include <iostream>
#include "../../graphics/renderer/renderer.h"

unsigned int SpriteManager::spriteVAOSmall, SpriteManager::spriteVBOSmall;
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

	Engine::renderer->SetDepthTest(false);

	//if (currentTexture != AssetManager::defaultTexture)
	//{
	Engine::renderer->EnableTextureUnit(0);
	Engine::renderer->BindTexture(AssetManager::defaultTexture);
	//}

	Engine::renderer->BindVertexArray(lineVAO);
	Engine::renderer->BindBuffer(Array_Buffer, lineVBO);
	Engine::renderer->SetBufferSubData(Array_Buffer, *vertices, sizeof(vertices));
	
	Engine::renderer->BindBuffer(Array_Buffer, 0);
	// render quad
	Engine::renderer->DrawArray(Triangles, 0, 6);
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

	float sizeFixer = 0.1f;
	Vector4 rgbaColor = color.GetRGBA().ToVector4();
	material->shader->SetShaderAttribut("color", rgbaColor);

	//Scale
	transformationMatrix[0].x *= w;
	transformationMatrix[0].y *= w;
	transformationMatrix[1].x *= h;
	transformationMatrix[1].y *= h;

	if ((transformationMatrix[1].y < 0 && transformationMatrix[0].x < 0) || (transformationMatrix[1].y > 0 && transformationMatrix[0].x > 0))
	{
		Engine::renderer->SetCullFace(Back);
	}
	else 
	{
		Engine::renderer->SetCullFace(Front);
	}

	//Move
	transformationMatrix[3].x *= -sizeFixer;
	transformationMatrix[3].y *= sizeFixer;

	UpdateMaterial(material, &transformationMatrix);
	Engine::renderer->SetDepthTest(false);

	// 
	//if (currentTexture != texture)
	//{
	currentTexture = texture;
	Engine::renderer->EnableTextureUnit(0);
	Engine::renderer->BindTexture(texture);
	//}

	Engine::renderer->BindVertexArray(spriteVAOSmall);
	Engine::renderer->BindBuffer(Array_Buffer, spriteVBOSmall);

	// render quad
	Engine::renderer->DrawArray(Triangles, 0, 6);
	Performance::AddDrawCall();
}

#pragma endregion

/// <summary>
/// Create a buffer for sprites
/// </summary>
void SpriteManager::CreateSpriteBuffers()
{
	spriteVAOSmall = Engine::renderer->GenerateVertexArray();
	spriteVBOSmall = Engine::renderer->GenerateBuffer();

	Engine::renderer->BindVertexArray(spriteVAOSmall);
	Engine::renderer->BindBuffer(Array_Buffer, spriteVBOSmall);
	Engine::renderer->SetBufferSize(Array_Buffer, Dynamic, 6 * 4);

	//Vertices attrib
	Engine::renderer->SetVertexAttribPointer(0, 2, 4, 0);
	Engine::renderer->SetVertexAttribArray(true, 0);

	//Uv attrib
	Engine::renderer->SetVertexAttribPointer(1, 2, 4, 2);
	Engine::renderer->SetVertexAttribArray(true, 1);

	float x = -0.5f;
	float vertices[6][4] = {
		{ x,     x + 1,   0.0f, 0.0f },
		{ x,     x,        0.0f, 1.0f },
		{ x + 1, x,       1.0f, 1.0f },

		{ x,     x + 1,    0.0f, 0.0f },
		{ x + 1, x,      1.0f, 1.0f },
		{ x + 1, x + 1,   1.0f, 0.0f }
	};
	Engine::renderer->SetBufferSubData(Array_Buffer, *vertices, sizeof(vertices));
	Engine::renderer->BindBuffer(Array_Buffer, 0);

	//////////////////////////////////////

	//////////////////////////////////////
	lineVAO = Engine::renderer->GenerateVertexArray();
	lineVBO = Engine::renderer->GenerateBuffer();

	Engine::renderer->BindVertexArray(lineVAO);
	Engine::renderer->BindBuffer(Array_Buffer, lineVBO);

	Engine::renderer->SetBufferSize(Array_Buffer, Dynamic, 6 * 5);

	//Vertices attrib
	Engine::renderer->SetVertexAttribPointer(0, 3, 5, 0);
	Engine::renderer->SetVertexAttribArray(true, 0);

	//Uv attrib
	Engine::renderer->SetVertexAttribPointer(1, 2, 5, 3);
	Engine::renderer->SetVertexAttribArray(true, 1);

	Engine::renderer->BindVertexArray(0);
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
	CreateSpriteBuffers();

	Debug::Print("---- Sprite System initiated ----");
}

SpriteBatch::SpriteBatch(Material* mat, const Texture* texture)
{
	material = mat;
	this->texture = texture;
	spriteVAO = Engine::renderer->GenerateVertexArray();
	spriteVBO = Engine::renderer->GenerateBuffer();

	Engine::renderer->BindVertexArray(spriteVAO);
	Engine::renderer->BindBuffer(Array_Buffer, spriteVBO);

	Engine::renderer->SetBufferSize(Array_Buffer, Dynamic, 6 * 4);

	//Vertices attrib
	Engine::renderer->SetVertexAttribPointer(0, 2, 4, 0);
	Engine::renderer->SetVertexAttribArray(true, 0);

	//Uv attrib
	Engine::renderer->SetVertexAttribPointer(1, 2, 4, 2);
	Engine::renderer->SetVertexAttribArray(true, 1);
}

void SpriteBatch::Draw(Color color)
{
	if (verticesCount != 0) 
	{
		glm::mat4 t = glm::mat4(1);
		UpdateMaterial(material, &t);

		Vector4 rgbaColor = color.GetRGBA().ToVector4();
		material->shader->SetShaderAttribut("color", rgbaColor);

		Engine::renderer->SetDepthTest(false);

		Engine::renderer->EnableTextureUnit(0);
		Engine::renderer->BindTexture(texture);

		Engine::renderer->BindBuffer(Array_Buffer, spriteVBO);
		Engine::renderer->SetBufferSubData(Array_Buffer, vertices, verticesCount * 4 * sizeof(float));

		Engine::renderer->BindVertexArray(spriteVAO);
		Engine::renderer->DrawArray(Triangles, 0, verticesCount);

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
	if (verticesCount != oldVerticeCount) 
	{
		if (vertices != nullptr) 
		{
			free(vertices);
		}
		vertices = (float*)malloc(verticesCount * 4 * sizeof(float));

		Engine::renderer->BindVertexArray(spriteVAO);
		Engine::renderer->BindBuffer(Array_Buffer, spriteVBO);
		Engine::renderer->SetBufferSize(Array_Buffer,Dynamic, verticesCount * 4);

		oldVerticeCount = verticesCount;
	}
}
