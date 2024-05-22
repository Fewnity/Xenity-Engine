// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include "menu.h"
#include <memory>
#include <string>

class FileReference;
class Component;
class GameObject;
class Transform;

class InspectorMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;

	/**
	* Check if the user wants to open the right click menu and open it if needed
	*/
	int CheckOpenRightClickPopup(std::shared_ptr<Component>& component, int& componentCount, int& componentIndex, const std::string& id);

	int CheckOpenRightClickPopupTransform(std::shared_ptr<Transform>& transform, const std::string& id);

	std::shared_ptr<FileReference> loadedPreview = nullptr;
	bool forceItemUpdate = false;

private:
	/**
	* Draw file preview part
	*/
	void DrawFilePreview();

	/**
	* Draw file info part
	*/
	void DrawFileInfo(const std::shared_ptr<FileReference>& selectedFileReference);

	/**
	* Draw selected gameobject infos
	*/
	void DrawGameObjectInfo(const std::shared_ptr <GameObject>& selectedGameObject);

	/**
	* Draw selected transform header
	*/
	void DrawTransformHeader(const std::shared_ptr <GameObject>& selectedGameObject);

	/**
	* Draw components headers
	*/
	void DrawComponentsHeaders(const std::shared_ptr <GameObject>& selectedGameObject);

	bool showAddComponentMenu = false;

	std::string previewText = "";
};

