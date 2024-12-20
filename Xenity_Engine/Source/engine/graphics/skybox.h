// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <memory>

#include <engine/api.h>
#include <engine/file_system/file_reference.h>
#include <engine/reflection/reflection.h>

class Texture;

/**
* @brief Skybox file class
*/
class API SkyBox : public FileReference, public std::enable_shared_from_this<SkyBox>
{
public:

	SkyBox();
	~SkyBox();

	std::shared_ptr<Texture> front = nullptr;
	std::shared_ptr<Texture> back = nullptr;
	std::shared_ptr<Texture> up = nullptr;
	std::shared_ptr<Texture> down = nullptr;
	std::shared_ptr<Texture> left = nullptr;
	std::shared_ptr<Texture> right = nullptr;

protected:
	friend class ProjectManager;

	/**
	* @brief Create a skybox
	* @param front Front face
	* @param back Back face
	* @param up Up face
	* @param down Down face
	* @param left Left face
	* @param right Right face
	*/
	SkyBox(const std::shared_ptr<Texture>& front, const std::shared_ptr<Texture>& back, const std::shared_ptr<Texture>& up, const std::shared_ptr<Texture>& down, const std::shared_ptr<Texture>& left, const std::shared_ptr<Texture>& right);

	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData(AssetPlatform platform) override;
	static std::shared_ptr<SkyBox> MakeSkyBox();
	void OnReflectionUpdated() override;
	void LoadFileReference() override;
	void UnloadFileReference() override;

	static constexpr int s_version = 1;
};