#pragma once
#include <vector>

class Shader;
class Texture;
class IDrawable;

class AssetManager
{
public:
	static void AddShader(Shader* shader);
	static void AddTexture(Texture* texture);
	static void AddDrawable(IDrawable* drawable);

	static void RemoveShader(Shader* shader);
	static void RemoveTexture(Texture* texture);
	static void RemoveDrawable(IDrawable* drawable);

	static int GetTextureCount();

	static std::vector<Shader *> shaders;
	static std::vector<Texture*> textures;
	static std::vector<IDrawable*> drawables;
private:
};

