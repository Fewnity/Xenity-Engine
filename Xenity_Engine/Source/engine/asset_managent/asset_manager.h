#pragma once
#include <vector>
#include <string>

// class Shader;
class Texture;
class IDrawable;
class Light;
// class Material;
// class MeshData;

class AssetManager
{
public:
	static void Init();
	// static void ResetMaterialsUpdates();

	// static void AddShader(Shader *shader);
	// static void AddMaterial(Material *material);
	static void AddTexture(Texture *texture);
	static void AddDrawable(IDrawable *drawable);
	static void AddLight(Light *light);
	// static void AddMeshData(MeshData *meshData);

	// static void RemoveShader(Shader *shader);
	// static void RemoveMaterial(Material *material);
	static void RemoveTexture(Texture *texture);
	static void RemoveDrawable(IDrawable *drawable);
	static void RemoveLight(Light *light);
	// static void RemoveMeshData(MeshData *meshData);

	// static Shader *GetShader(const int index);
	// static Material *GetMaterial(const int index);
	static Texture *GetTexture(const int index);
	// static Material *GetMaterialByName(const std::string name);
	static Texture *GetTextureByName(const std::string name);
	static IDrawable *GetDrawable(const int index);
	static Light *GetLight(const int index);
	// static MeshData *GetMeshData(const int index);
	// static MeshData *GetMeshData(const std::string path);

	// static int GetShaderCount();
	// static int GetMaterialCount();
	static int GetTextureCount();
	static int GetDrawableCount();
	static int GetLightCount();
	// static int GetMeshDataCount();

	// static Material *default3DMaterial;
	// static Material *default2DMaterial;
	// static Material *defaultUIMaterial;

	static Texture *defaultTexture;

private:
	// static int shaderCount;
	// static int materialCount;
	static int textureCount;
	static int drawableCount;
	static int lightCount;
	// static int meshDataCount;

	// static std::vector<Shader *> shaders;
	// static std::vector<Material *> materials;
	static std::vector<Texture *> textures;
	static std::vector<IDrawable *> drawables;
	static std::vector<Light *> lights;
	// static std::vector<MeshData *> meshesData;
};
