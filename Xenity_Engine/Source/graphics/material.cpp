#include "material.h"
#include "shader.h"
#include "../asset_manager.h"
#include <glad/glad.h>
#include <iostream>
#include "../debug/performance.h"

#pragma region Constructors / Destructor

Material::Material()
{
	AssetManager::AddMaterial(this);
}

Material::~Material()
{
	AssetManager::RemoveMaterial(this);
}

#pragma endregion

#pragma region Attributs setters

/// <summary>
/// Add a Vector2 attribut
/// </summary>
/// <param name="attribut">Attribut name</param>
/// <param name="value">Vector2</param>
void Material::SetAttribut(const std::string attribut, const Vector2 value)
{
	uniformsVector2.insert(std::pair <std::string, Vector2>(attribut, value));
}

/// <summary>
/// Add a Vector3 attribut
/// </summary>
/// <param name="attribut">Attribut name</param>
/// <param name="value">Vector3</param>
void Material::SetAttribut(const std::string attribut, const Vector3 value)
{
	uniformsVector3.insert(std::pair <std::string, Vector3>(attribut, value));
}

/// <summary>
/// Add a Vector4 attribut
/// </summary>
/// <param name="attribut">Attribut name</param>
/// <param name="value">Vector4</param>
void Material::SetAttribut(const std::string attribut, const Vector4 value)
{
	uniformsVector4.insert(std::pair <std::string, Vector4>(attribut, value));
}

/// <summary>
/// Add a Texture attribut
/// </summary>
/// <param name="attribut">Attribut name</param>
/// <param name="value">Texture pointer</param>
void Material::SetAttribut(const std::string attribut, Texture* value)
{
	uniformsTextures.insert(std::pair <std::string, Texture*>(attribut, value));
}

/// <summary>
/// Add a float attribut
/// </summary>
/// <param name="attribut">Attribut name</param>
/// <param name="value">float</param>
void Material::SetAttribut(const std::string attribut, const float value)
{
	uniformsFloat.insert(std::pair <std::string, float>(attribut, value));
}

/// <summary>
/// Add an int attribut
/// </summary>
/// <param name="attribut">Attribut name</param>
/// <param name="value">int</param>
void Material::SetAttribut(const std::string attribut, const int value)
{
	uniformsInt.insert(std::pair <std::string, int>(attribut, value));
}

#pragma endregion

/// <summary>
/// Use the material to draw something
/// </summary>
void Material::Use()
{
	shader->Use();
}

/// <summary>
/// Update the material 
/// </summary>
void Material::Update()
{
	if (shader != nullptr) 
	{
		Use();
		Performance::AddMaterialUpdate();

		//Send all uniforms

		int textureIndex = 0;
		for (const auto& kv : uniformsTextures)
		{
			//Enable each textures units
			glActiveTexture(GL_TEXTURE0 + textureIndex);
			glBindTexture(GL_TEXTURE_2D, kv.second->GetTextureId());
			shader->SetShaderAttribut(kv.first, textureIndex);
			textureIndex++;
		}
		for (const auto& kv : uniformsVector3) 
		{
			shader->SetShaderAttribut(kv.first, kv.second);
		}
		for (const auto& kv : uniformsInt) 
		{
			shader->SetShaderAttribut(kv.first, kv.second);
		}
		for (const auto& kv : uniformsFloat) 
		{
			shader->SetShaderAttribut(kv.first, kv.second);
		}

		updated = true;
	}
}
