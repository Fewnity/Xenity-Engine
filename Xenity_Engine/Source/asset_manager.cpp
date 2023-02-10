#include "asset_manager.h"

#include "graphics/texture.h"
#include "graphics/shader.h"
#include "graphics/mesh.h"
#include "lighting/lighting.h"

std::vector<Shader*> AssetManager::shaders;
std::vector<Texture*> AssetManager::textures;
std::vector<IDrawable*> AssetManager::drawables;
std::vector<Light*> AssetManager::lights;

/// <summary>
/// Add a shader in the shader list
/// </summary>
/// <param name="shader"></param>
void AssetManager::AddShader(Shader* shader)
{
	shaders.push_back(shader);
}

/// <summary>
/// Add a texture in the texture list
/// </summary>
/// <param name="texture"></param>
void AssetManager::AddTexture(Texture* texture)
{
	textures.push_back(texture);
}

/// <summary>
/// Add a drawable in the drawable list
/// </summary>
/// <param name="drawable"></param>
void AssetManager::AddDrawable(IDrawable* drawable)
{
	drawables.push_back(drawable);
}

/// <summary>
/// Add a light in the light list
/// </summary>
/// <param name="light"></param>
void AssetManager::AddLight(Light* light)
{
	lights.push_back(light);
}

/// <summary>
/// Remove a shader from the shader list
/// </summary>
/// <param name="shader"></param>
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

/// <summary>
/// Remove a texture from the texture list
/// </summary>
/// <param name="texture"></param>
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

/// <summary>
/// Remove a drawable from the drawable list
/// </summary>
/// <param name="drawable"></param>
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

/// <summary>
/// Remove a light from the light list
/// </summary>
/// <param name="light"></param>
void AssetManager::RemoveLight(Light* light)
{
	int lightCount = lights.size();
	int lightIndex = 0;
	bool found = false;
	for (int i = 0; i < lightCount; i++)
	{
		if (lights[i] == light) {
			found = true;
			lightIndex = i;
			break;
		}
	}

	if (found)
		lights.erase(lights.begin() + lightIndex);
}

/// <summary>
/// Get texture count
/// </summary>
/// <returns></returns>
int AssetManager::GetTextureCount()
{
	return textures.size();
}
