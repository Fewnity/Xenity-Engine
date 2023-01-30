#pragma once
#include <vector>

class Shader;
class Texture;
class Mesh;

class AssetManager
{
public:
	static void AddShader(Shader* shader);
	static void AddTexture(Texture* texture);
	static void AddMesh(Mesh* mesh);

	static void RemoveShader(Shader* shader);
	static void RemoveTexture(Texture* texture);
	static void RemoveMesh(Mesh* mesh);

	static std::vector<Shader *> shaders;
	static std::vector<Texture*> textures;
	static std::vector<Mesh*> meshs;
private:
};

