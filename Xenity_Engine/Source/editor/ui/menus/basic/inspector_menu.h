// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <memory>
#include <string>

#include <editor/ui/menus/menu.h>

#include <engine/platform.h>

class FileReference;
class Component;
class GameObject;
class Transform;

class InspectorMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;

	std::shared_ptr<FileReference> loadedPreview = nullptr;

	bool forceItemUpdate = false;

	AssetPlatform platformView = AssetPlatform::AP_Standalone;

private:
	/**
	* Check if the user wants to open the right click menu and open it if needed
	*/
	int CheckOpenRightClickPopup(Component& component, int& componentCount, int& componentIndex, const std::string& id);

	int CheckOpenRightClickPopupTransform(Transform& transform, const std::string& id);

	/**
	* Draw file preview part
	*/
	void DrawFilePreview();

	/**
	* Draw file info part
	*/
	void DrawFileInfo(FileReference& selectedFileReference);

	/**
	* Draw selected gameobject infos
	*/
	void DrawGameObjectInfo(GameObject& selectedGameObject);

	/**
	* Draw selected transform header
	*/
	void DrawTransformHeader(const GameObject& selectedGameObject);

	/**
	* Draw components headers
	*/
	void DrawComponentsHeaders(const GameObject& selectedGameObject);

	bool showAddComponentMenu = false;

	std::string previewText = "";
	bool isPlayingAudio = false;
	bool areWindowsFocused = false;

	void StopAudio();
};

