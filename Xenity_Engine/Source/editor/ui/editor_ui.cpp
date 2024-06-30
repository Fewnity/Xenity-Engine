// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(EDITOR)
#include "editor_ui.h"

#include <variant>
#include <Windows.h>
#include <ShObjIdl.h>
#include <SDL3/SDL_video.h>

// ImGui
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_internal.h>

#include <editor/ui/editor_ui.h>
#include <editor/editor.h>
#include <engine/file_system/file_system.h>
#include <engine/file_system/file.h>
#include <engine/graphics/texture.h>
#include <engine/ui/window.h>
#include <engine/debug/debug.h>
#include <engine/engine_settings.h>

using json = nlohmann::json;

int EditorUI::uiId = 0;

std::map<std::string, std::shared_ptr<Texture>> EditorUI::componentsIcons;
std::vector<std::shared_ptr<Texture>> EditorUI::icons;
MultiDragData EditorUI::multiDragData;

float EditorUI::uiScale = 1;
std::shared_ptr<Menu> EditorUI::currentSelectAssetMenu;
bool EditorUI::isEditingElement = false;

CopyType EditorUI::currentCopyType;
json EditorUI::copiedComponentJson;
std::string EditorUI::copiedComponentName;
Event<>* EditorUI::onValueChangedEvent = nullptr;

#pragma region Initialisation

int EditorUI::Init()
{
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigWindowsMoveFromTitleBarOnly = true;
	std::shared_ptr<File> fontFile = FileSystem::MakeFile("Roboto Regular.ttf");
	if (fontFile->CheckIfExist()) 
	{
		io.Fonts->AddFontFromFileTTF(fontFile->GetPath().c_str(), 30);
	}
	else 
	{
		Debug::PrintError("[EditorUI::Init] Fail to load font file:" + fontFile->GetPath(), true);
		return (int)EditorUIError::EDITOR_UI_ERROR_MISSING_FONT;
	}

	icons.resize((int)IconName::Icon_Count);

	// File types icons
	LoadEditorIcon(IconName::Icon_File, "icons/text.png");
	LoadEditorIcon(IconName::Icon_Folder, "icons/folder.png");
	LoadEditorIcon(IconName::Icon_Scene, "icons/belt.png");
	LoadEditorIcon(IconName::Icon_Image, "icons/image.png");
	LoadEditorIcon(IconName::Icon_Mesh, "icons/3d.png");
	LoadEditorIcon(IconName::Icon_Code, "icons/code.png");
	LoadEditorIcon(IconName::Icon_Header, "icons/header.png");
	LoadEditorIcon(IconName::Icon_Audio, "icons/audio.png");
	LoadEditorIcon(IconName::Icon_Font, "icons/font.png");
	LoadEditorIcon(IconName::Icon_Sky, "icons/sky.png");
	LoadEditorIcon(IconName::Icon_Material, "icons/material.png");
	LoadEditorIcon(IconName::Icon_Shader, "icons/shader.png");

	// 3D icons
	LoadEditorIcon(IconName::Icon_Camera, "icons/camera.png");
	LoadEditorIcon(IconName::Icon_Audio_Source, "icons/audio_source.png");
	LoadEditorIcon(IconName::Icon_Point_Light, "icons/point_light.png");
	LoadEditorIcon(IconName::Icon_Sun_Light, "icons/sun_light.png");
	LoadEditorIcon(IconName::Icon_Spot_Light, "icons/spot_light.png");

	// Play Pause Stop icons...
	LoadEditorIcon(IconName::Icon_Play, "icons/play.png");
	LoadEditorIcon(IconName::Icon_Pause, "icons/pause.png");
	LoadEditorIcon(IconName::Icon_Stop, "icons/stop.png");

	// Scene tab icons
	LoadEditorIcon(IconName::Icon_Camera_Move, "icons/camera_move.png");
	LoadEditorIcon(IconName::Icon_Move, "icons/move.png");
	LoadEditorIcon(IconName::Icon_Rotate, "icons/rotate.png");
	LoadEditorIcon(IconName::Icon_Scale, "icons/scale.png");

	// Platform icons
	LoadEditorIcon(IconName::Icon_Platform_Windows, "icons/platform_windows.png");
	LoadEditorIcon(IconName::Icon_Platform_PSP, "icons/platform_psp.png");
	LoadEditorIcon(IconName::Icon_Platform_PSVITA, "icons/platform_psvita.png");
	LoadEditorIcon(IconName::Icon_Platform_PS2, "icons/platform_ps2.png");
	LoadEditorIcon(IconName::Icon_Platform_PS3, "icons/platform_ps3.png");
	LoadEditorIcon(IconName::Icon_Platform_PS4, "icons/platform_ps4.png");

	LoadComponentIcon("BoxCollider", "icons/components/box_collider.png");
	LoadComponentIcon("SpriteRenderer", "icons/components/sprite_renderer.png");
	LoadComponentIcon("Lod", "icons/components/lod.png");
	LoadComponentIcon("MeshRenderer", "icons/components/mesh_renderer.png");
	LoadComponentIcon("ParticleSystem", "icons/components/particles.png");
	LoadComponentIcon("Canvas", "icons/components/canvas.png");
	LoadComponentIcon("TextRenderer", "icons/components/text_renderer_canvas.png");
	LoadComponentIcon("TextMesh", "icons/components/text_renderer.png");
	LoadComponentIcon("AudioSource", "icons/components/audio_source.png");
	LoadComponentIcon("RigidBody", "icons/components/rigidbody.png");
	LoadComponentIcon("Transform", "icons/components/transform.png");
	LoadComponentIcon("RectTransform", "icons/components/rect_transform.png");
	LoadComponentIcon("BillboardRenderer", "icons/components/billboard.png");
	LoadComponentIcon("Default", "icons/components/default.png");
	LoadComponentIcon("Camera", "icons/components/camera.png");
	LoadComponentIcon("Light", "icons/components/point_light.png");


	Debug::Print("---- Editor UI initiated ----", true);
	return 0;
}

void EditorUI::Draw()
{
	NewFrame();
	Render();
}

#pragma endregion

#pragma region Update

ImVec4 normalColor{ 0.5f, 0.5f, 0.5f, 0.5f };
ImVec4 pressedColor{ 0.3f, 0.3f, 0.3f, 0.5f };
ImVec4 hoverColor{ 0.4f, 0.4f, 0.4f, 0.6f };

void EditorUI::SetButtonColor(bool isSelected)
{
	ImVec4 currentColor = normalColor;
	const Vector4 color = EngineSettings::values.secondaryColor.GetRGBA().ToVector4() / 2.0f;
	const Vector4 colorHover = EngineSettings::values.secondaryColor.GetRGBA().ToVector4();
	pressedColor = ImVec4(normalColor.x + color.x, normalColor.y + color.y, normalColor.z + color.z, normalColor.w + 0.2f);
	hoverColor = ImVec4(normalColor.x + colorHover.x, normalColor.y + colorHover.y, normalColor.z + colorHover.z, normalColor.w + 0.2f);
	if (isSelected)
	{
		currentColor = pressedColor;
	}
	ImGui::PushStyleColor(ImGuiCol_Button, currentColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, pressedColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
}

void EditorUI::EndButtonColor()
{
	ImGui::PopStyleColor(3);
}

std::string EditorUI::GetPrettyVariableName(std::string variableName)
{
	variableName[0] = toupper(variableName[0]);
	size_t nameSize = variableName.size();
	bool addSpace = false;
	for (size_t i = 1; i < nameSize - 1; i++)
	{
		if (isupper(variableName[i]))
		{
			if (addSpace)
				addSpace = false;
			else
				addSpace = true;
		}
		else if (addSpace)
		{
			variableName.insert(i - 1, " ");
			nameSize++;
			i++;
			addSpace = false;
		}
	}
	return variableName;
}

/**
* Create a new frame for the editor's UI
*/
void EditorUI::NewFrame()
{
	EditorUI::UpdateUIScale();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
	uiId = 0;
}

void EditorUI::UpdateUIScale()
{
	const int index = SDL_GetDisplayForWindow(Window::window);
	if (index >= 0)
	{
		uiScale = SDL_GetDisplayContentScale(index);
		if (uiScale == 0)
		{
			uiScale = 1;
		}
	}

	ImGui::GetIO().FontGlobalScale = 0.5f * uiScale;
}

void EditorUI::LoadEditorIcon(IconName iconName, const std::string& path)
{
	std::shared_ptr<Texture> fileIcon = Texture::MakeTexture();
	fileIcon->file = FileSystem::MakeFile(path);
	fileIcon->SetWrapMode(WrapMode::ClampToEdge);
	fileIcon->LoadFileReference();
	icons[(int)iconName] = std::move(fileIcon);
}

void EditorUI::LoadComponentIcon(std::string iconName, const std::string& path)
{
	std::shared_ptr<Texture> fileIcon = Texture::MakeTexture();
	fileIcon->file = FileSystem::MakeFile(path);
	fileIcon->SetWrapMode(WrapMode::ClampToEdge);
	fileIcon->LoadFileReference();
	componentsIcons[iconName] = std::move(fileIcon);
}

/**
* Render the editor's UI
*/
void EditorUI::Render()
{
	isEditingElement = ImGui::IsAnyItemActive();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}
}

void EditorUI::SetRoundedCorner(float value)
{
	ImGui::GetStyle().WindowRounding = value;
}

#pragma endregion

#pragma region Low Level Draw Functions

std::string EditorUI::GenerateItemId()
{
	const std::string itemId = "##" + std::to_string(uiId);
	uiId++;
	return itemId;
}

void EditorUI::DrawTextCentered(const std::string& text)
{
	const float windowWidth = ImGui::GetWindowSize().x;
	const float textWidth = ImGui::CalcTextSize(text.c_str()).x;
	ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
	ImGui::Text("%s", text.c_str());
}

#pragma endregion

#endif