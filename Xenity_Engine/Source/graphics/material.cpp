#include "material.h"

#include "shader.h"
#include "../asset_manager.h"
#include <glad/glad.h>
#include <iostream>

Material::Material()
{
	AssetManager::AddMaterial(this);
}

Material::~Material()
{
	AssetManager::RemoveMaterial(this);
}

void Material::Use() {
	Update();
}

void Material::SetAttribut(std::string attribut, Vector2 value)
{
	uniformsVector2.insert(std::pair <std::string, Vector2>(attribut, value));
}

void Material::SetAttribut(std::string attribut, Vector3 value)
{
	uniformsVector3.insert(std::pair <std::string, Vector3>(attribut, value));
}

void Material::SetAttribut(std::string attribut, Vector4 value)
{
	uniformsVector4.insert(std::pair <std::string, Vector4>(attribut, value));
}

void Material::SetAttribut(std::string attribut, Texture* value)
{
	uniformsTextures.insert(std::pair <std::string, Texture*>(attribut, value));
}

void Material::SetAttribut(std::string attribut, float value)
{
	uniformsFloat.insert(std::pair <std::string, float>(attribut, value));
}

void Material::SetAttribut(std::string attribut, int value)
{
	uniformsInt.insert(std::pair <std::string, int>(attribut, value));
}

void Material::Update()
{
	if (shader != nullptr) 
	{
		shader->Use();

		int textureIndex = 0;
		for (const auto& kv : uniformsTextures) 
		{
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
	}
}
