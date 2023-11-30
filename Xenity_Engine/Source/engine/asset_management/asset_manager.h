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
#include <engine/file_system/file_type.h>

class Texture;
class Light;
class FileReference;
class Reflective;
class Material;
class Shader;

class API AssetManager
{
public:
	static void Init();

	static void AddMaterial(Material* material);
	static void AddReflection(Reflective* reflection);
	static void AddFileReference(const std::shared_ptr <FileReference>& fileReference);
	static void AddLight(const std::weak_ptr<Light>& light);

	static void RemoveMaterial(const Material* material);
	static void RemoveReflection(const Reflective* reflection);
	static void RemoveAllFileReferences();
	static void RemoveFileReference(const std::shared_ptr <FileReference>& fileReference);
	static void RemoveLight(const std::weak_ptr<Light>& light);
	static void ForceDeleteFileReference(const std::shared_ptr<FileReference>& fileReference);

	static Material* GetMaterial(const int index);
	static Reflective* GetReflectiveData(const int index);
	static std::shared_ptr<FileReference> GetFileReference(const int index);
	static std::weak_ptr<Light> GetLight(const int index);

	static void RemoveUnusedFiles();

	static int GetMaterialCount();
	static int GetReflectionCount();
	static int GetFileReferenceCount();
	static int GetDrawableCount();
	static int GetLightCount();

	static std::shared_ptr <Texture> defaultTexture;

	static std::string GetDefaultFileData(FileType fileType);

	static std::shared_ptr<Shader> shader;
	static std::shared_ptr<Shader> unlitShader;
	static std::shared_ptr<Shader> lineShader;
	static std::shared_ptr<Material> standardMaterial;
	static std::shared_ptr<Material> unlitMaterial;
	static std::shared_ptr<Material> lineMaterial;

private:
	static int materialCount;
	static int reflectionCount;
	static int fileReferenceCount;
	static int lightCount;

	static std::vector<Material*> materials;
	static std::vector<Reflective*> reflections;
	static std::vector<std::shared_ptr<FileReference>> fileReferences;
	static std::vector<std::weak_ptr<Light>> lights;
};
