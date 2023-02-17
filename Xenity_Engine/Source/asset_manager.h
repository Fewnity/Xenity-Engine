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
	static void Init();
	static void ResetMaterialsUpdates();

	static void AddShader(Shader* shader);
	static void AddMaterial(Material* material);
	static void AddTexture(Texture* texture);
	static void AddDrawable(IDrawable* drawable);
	static void AddLight(Light* light);
	static void AddMeshData(MeshData* meshData);

	static void RemoveShader(Shader* shader);
	static void RemoveMaterial(Material* material);
	static void RemoveTexture(Texture* texture);
	static void RemoveDrawable(IDrawable* drawable);
	static void RemoveLight(Light* light);
	static void RemoveMeshData(MeshData* meshData);

	static Shader* GetShader(int index);
	static Material* GetMaterial(int index);
	static Texture* GetTexture(int index);
	static IDrawable* GetDrawable(int index);
	static Light* GetLight(int index);

	static int GetShaderCount();
	static int GetMaterialCount();
	static int GetTextureCount();
	static int GetDrawableCount();
	static int GetLightCount();

	static Material* default3DMaterial;
	static Material* default2DMaterial;
	static Material* defaultUIMaterial;

	static Texture* defaultTexture;

private:
	static int shaderCount;
	static int materialCount;
	static int textureCount;
	static int drawableCount;
	static int lightCount;

	static std::vector<Shader *> shaders;
	static std::vector<Material*> materials;
	static std::vector<Texture*> textures;
	static std::vector<IDrawable*> drawables;
	static std::vector<Light*> lights;
	static std::vector<MeshData*> meshesData;
};

