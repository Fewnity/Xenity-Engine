#pragma once

/**
 * [Internal]
 */

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

class AssetManager
{
public:
	/**
	* @brief Initializes the asset manager
	*/
	static void Init();

	/**
	* @brief Adds a material
	* @param material The material to add
	*/
	static void AddMaterial(Material* material);

	/**
	* @brief Used in component constructor when the component has files references stored in variables
	* @param reflection The reflection to add
	*/
	static void AddReflection(Reflective* reflection);

	/**
	* @brief Adds a file reference
	* @param fileReference The file reference to add
	*/
	static void AddFileReference(const std::shared_ptr <FileReference>& fileReference);

	/**
	* @brief Adds a light
	* @param light The light to add
	*/
	static void AddLight(const std::weak_ptr<Light>& light);

	/**
	* @brief Removes a material
	* @param material The material to remove
	*/
	static void RemoveMaterial(const Material* material);

	/**
	* @brief Used in component destructor when the component has files references stored in variables
	* @param reflection The reflection to remove
	*/
	static void RemoveReflection(const Reflective* reflection);

	/**
	* @brief Removes all file references
	*/
	static void RemoveAllFileReferences();

	/**
	* @brief Removes a file reference
	* @param fileReference The file reference to remove
	*/
	static void RemoveFileReference(const std::shared_ptr <FileReference>& fileReference);

	/**
	* @brief Removes a light
	* @param light The light to remove
	*/
	static void RemoveLight(const std::weak_ptr<Light>& light);

	/**
	* @brief Remove all reference of a file reference (check all reflections and remove the reference if there is one)
	*/
	static void ForceDeleteFileReference(const std::shared_ptr<FileReference>& fileReference);

	/**
	* @brief Get a material by index
	* @param index The index of the material
	* @return The material
	*/
	static Material* GetMaterial(const int index);

	/**
	* @brief Get a reflection by index
	* @param index The index of the reflection
	* @return The reflection
	*/
	static Reflective* GetReflectiveData(const int index);

	/**
	* @brief Get a file reference by index
	* @param index The index of the file reference
	* @return The file reference
	*/
	static std::shared_ptr<FileReference> GetFileReference(const int index);

	/**
	* @brief Get a light by index
	* @param index The index of the light
	* @return The light
	*/
	static std::weak_ptr<Light> GetLight(const int index);

	/**
	* @brief Remove all unused files from the file references list
	*/
	static void RemoveUnusedFiles();

	/**
	* @brief Get the number of materials
	*/
	static int GetMaterialCount();

	/**
	* @brief Get the number of reflections
	*/
	static int GetReflectionCount();

	/**
	* @brief Get the number of file references
	*/
	static int GetFileReferenceCount();

	/**
	* @brief Get the number of lights
	*/
	static int GetLightCount();

	/**
	* @brief Get the default file data for a file type
	* @param fileType The file type
	* @return The default file data
	*/
	static std::string GetDefaultFileData(FileType fileType);

	static std::shared_ptr <Texture> defaultTexture;
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
