#include "asset_manager.h"

#include "graphics/texture.h"
#include "graphics/shader.h"
#include "graphics/mesh.h"

std::vector<Shader*> AssetManager::shaders;
std::vector<Texture*> AssetManager::textures;
std::vector<IDrawable*> AssetManager::drawables;

void AssetManager::AddShader(Shader* shader)
{
	shaders.push_back(shader);
}

void AssetManager::AddTexture(Texture* texture)
{
	textures.push_back(texture);
}

void AssetManager::AddDrawable(IDrawable* drawable)
{
	drawables.push_back(drawable);
}

void AssetManager::RemoveShader(Shader* shader)
{
	int shaderCount = textures.size();
	int shaderIndex = 0;
	bool found = false;
	for (int i = 0; i < shaderCount; i++)
	{
		if (shaders[i] == shader) {
			found = true;
			shaderIndex = i;
			break;
		}
	}
	if (found)
		shaders.erase(shaders.begin() + shaderIndex);
}

void AssetManager::RemoveTexture(Texture* texture)
{
	int textureCount = textures.size();
	int textureIndex = 0;
	bool found = false;
	for (int i = 0; i < textureCount; i++)
	{
		if (textures[i] == texture) {
			found = true;
			textureIndex = i;
			break;
		}
	}
	if (found)
		textures.erase(textures.begin() + textureIndex);
}

void AssetManager::RemoveDrawable(IDrawable* drawable)
{
	int drawableCount = drawables.size();
	int drawableIndex = 0;
	bool found = false;
	for (int i = 0; i < drawableCount; i++)
	{
		if (drawables[i] == drawable) {
			found = true;
			drawableIndex = i;
			break;
		}
	}
	if (found)
		drawables.erase(drawables.begin() + drawableIndex);
}

int AssetManager::GetTextureCount()
{
	return textures.size();
}
