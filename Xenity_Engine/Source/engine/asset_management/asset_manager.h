// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal] Class not visible to users
 */

#include <vector>
#include <string>
#include <memory>

#include <engine/file_system/file_type.h>
#include <engine/asset_management/project_manager.h>

class Texture;
class Light;
class FileReference;
class Reflective;
class Material;
class Shader;

/**
* @brief Class to keep in memory some objects
*/
class AssetManager
{
public:
	/**
	* @brief Initializes the asset manager
	*/
	static void Init();

	/**
	* @brief Event called when a project is loaded
	*/
	static void OnProjectLoaded();

	/**
	* @brief Event called when a project is unloaded
	*/
	static void OnProjectUnloaded();

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
	static void AddLight(Light* light);

	static void UpdateLightIndices();

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
	static void RemoveLight(Light* light);

	/**
	* @brief Remove all reference of a file reference (check all reflections and remove the reference if there is one)
	*/
	static void ForceDeleteFileReference(const std::shared_ptr<FileReference>& fileReference);

	/**
	* @brief Get a material by index
	* @param index The index of the material
	* @return The material
	*/
	static inline Material* GetMaterial(const int index)
	{
		XASSERT(index < materials.size(), "[AssetManager::GetMaterial] index is invalid");
		return materials[index];
	}

	/**
	* @brief Get a reflection by index
	* @param index The index of the reflection
	* @return The reflection
	*/
	static inline Reflective* GetReflectiveData(const int index)
	{
		XASSERT(index < reflections.size(), "[AssetManager::GetReflectiveData] index is invalid");
		return reflections[index];
	}

	/**
	* @brief Get a file reference by index
	* @param index The index of the file reference
	* @return The file reference
	*/
	static inline const std::shared_ptr<FileReference>& GetFileReference(const int index)
	{
		XASSERT(index < fileReferences.size(), "[AssetManager::GetFileReference] index is invalid");
		return fileReferences[index];
	}

	/**
	* @brief Get a light by index
	* @param index The index of the light
	* @return The light
	*/
	static inline const Light* GetLight(const int index)
	{
		XASSERT(index < lights.size(), "[AssetManager::GetLight] index is invalid");
		return lights[index];
	}

	static inline const std::vector<Light*>& GetLights()
	{
		return lights;
	}

	/**
	* @brief Remove all unused files from the file references list
	*/
	static void RemoveUnusedFiles();

	/**
	* @brief Get the number of materials
	*/
	static inline int GetMaterialCount()
	{
		return materialCount;
	}

	/**
	* @brief Get the number of reflections
	*/
	static inline int GetReflectionCount()
	{
		return reflectionCount;
	}

	/**
	* @brief Get the number of file references
	*/
	static inline int GetFileReferenceCount()
	{
		return fileReferenceCount;
	}

	/**
	* @brief Get the number of lights
	*/
	static inline int GetLightCount()
	{
		return lightCount;
	}

	/**
	* @brief Get the default file data for a file type
	* @param fileType The file type
	* @return The default file data
	*/
	static std::string GetDefaultFileData(FileType fileType);

	static std::shared_ptr <Texture> defaultTexture;
	static std::shared_ptr<Shader> standardShader;
	static std::shared_ptr<Shader> standardOneLightEachShader;
	static std::shared_ptr<Shader> standardOnePointLightShader;
	static std::shared_ptr<Shader> standardOneSpotLightShader;
	static std::shared_ptr<Shader> standardOneDirectionalLightShader;
	static std::shared_ptr<Shader> standardVertexLightShader;
	static std::shared_ptr<Shader> unlitShader;
	static std::shared_ptr<Shader> lineShader;
	static std::shared_ptr<Material> standardMaterial;
	static std::shared_ptr<Material> standardOneLightEachMaterial;
	static std::shared_ptr<Material> standardOnePointLightMaterial;
	static std::shared_ptr<Material> standardOneSpotLightMaterial;
	static std::shared_ptr<Material> standardOneDirectionalLightMaterial;
	static std::shared_ptr<Material> standardVertexLightMaterial;
	static std::shared_ptr<Material> unlitMaterial;
	static std::shared_ptr<Material> lineMaterial;

	template <typename T>
	static std::shared_ptr<T> LoadEngineAsset(const std::string& filePath)
	{
		return std::dynamic_pointer_cast<T>(ProjectManager::GetFileReferenceByFilePath(filePath));
	}
private:


	static int materialCount;
	static int reflectionCount;
	static int fileReferenceCount;
	static int lightCount;

	static std::vector<Material*> materials;
	static std::vector<Reflective*> reflections;
	static std::vector<std::shared_ptr<FileReference>> fileReferences;
	static std::vector<Light*> lights;
};
