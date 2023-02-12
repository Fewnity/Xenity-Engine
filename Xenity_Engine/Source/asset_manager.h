#pragma once
#include <vector>

class Shader;
class Texture;
class IDrawable;
class Light;
class Material;

class AssetManager
{
public:
	static void AddShader(Shader* shader);
	static void AddMaterial(Material* material);
	static void AddTexture(Texture* texture);
	static void AddDrawable(IDrawable* drawable);
	static void AddLight(Light* light);

	static void RemoveShader(Shader* shader);
	static void RemoveMaterial(Material* material);
	static void RemoveTexture(Texture* texture);
	static void RemoveDrawable(IDrawable* drawable);
	static void RemoveLight(Light* light);

	static int GetTextureCount();

	static std::vector<Shader *> shaders;
	static std::vector<Material*> materials;
	static std::vector<Texture*> textures;
	static std::vector<IDrawable*> drawables;
	static std::vector<Light*> lights;

private:
};

