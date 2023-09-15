#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once
#include <vector>
#include <string>
#include <memory>

// class Shader;
class Texture;
class IDrawable;
class Light;
class FileReference;
class Reflection;
// class Material;
// class MeshData;

class API AssetManager
{
public:
	static void Init();
	// static void ResetMaterialsUpdates();

	// static void AddShader(Shader *shader);
	// static void AddMaterial(Material *material);
	static void AddReflection(Reflection* reflection);
	static void AddFileReference(std::shared_ptr <FileReference> fileReference);
	static void AddDrawable(std::weak_ptr<IDrawable> drawable);
	static void AddLight(std::weak_ptr<Light> light);
	// static void AddMeshData(MeshData *meshData);

	// static void RemoveShader(Shader *shader);
	// static void RemoveMaterial(Material *material);
	static void RemoveReflection(Reflection* reflection);
	static void RemoveAllFileReferences();
	static void RemoveFileReference(std::shared_ptr <FileReference> fileReference);
	static void RemoveDrawable(std::weak_ptr<IDrawable> drawable);
	static void RemoveLight(std::weak_ptr<Light> light);
	static void ForceDeleteFileReference(std::shared_ptr<FileReference> fileReference);
	// static void RemoveMeshData(MeshData *meshData);

	// static Shader *GetShader(const int index);
	// static Material *GetMaterial(const int index);
	static Reflection* GetReflection(const int index);
	static std::shared_ptr<FileReference> GetFileReference(const int index);
	// static Material *GetMaterialByName(const std::string name);
	static std::weak_ptr<IDrawable> GetDrawable(const int index);
	static std::weak_ptr<Light> GetLight(const int index);
	// static MeshData *GetMeshData(const int index);
	// static MeshData *GetMeshData(const std::string path);

	// static int GetShaderCount();
	// static int GetMaterialCount();
	static int GetReflectionCount();
	static int GetFileReferenceCount();
	static int GetDrawableCount();
	static int GetLightCount();
	// static int GetMeshDataCount();

	// static Material *default3DMaterial;
	// static Material *default2DMaterial;
	// static Material *defaultUIMaterial;

	static std::shared_ptr <Texture> defaultTexture;
private:
	// static int shaderCount;
	// static int materialCount;
	static int reflectionCount;
	static int fileReferenceCount;
	static int drawableCount;
	static int lightCount;
	// static int meshDataCount;

	// static std::vector<Shader *> shaders;
	// static std::vector<Material *> materials;
	static std::vector<Reflection*> reflections;
	static std::vector<std::shared_ptr<FileReference>> fileReferences;
	static std::vector<std::weak_ptr<IDrawable>> drawables;
	static std::vector<std::weak_ptr<Light>> lights;
	// static std::vector<MeshData *> meshesData;
};
