#if defined(EDITOR)
#include "editor_ui.h"

#include <variant>
#include <Windows.h>
#include <ShObjIdl.h>
#include <SDL2/SDL_video.h>

// ImGui
#include <imgui/imgui_impl_sdl2.h>
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

int EditorUI::uiId = 0;

std::vector<std::shared_ptr<Texture>> EditorUI::icons;
MultiDragData EditorUI::multiDragData;

float EditorUI::uiScale = 1;

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
		Debug::PrintError("[EditorUI::Init] Fail to load font file:" + fontFile->GetPath());
		return EDITOR_UI_ERROR_MISSING_FONT;
	}

	for (int i = 0; i < Icon_Count; i++)
	{
		icons.emplace_back(std::shared_ptr<Texture>());
	}
	LoadEditorIcon(Icon_File, "icons/text.png");
	LoadEditorIcon(Icon_Folder, "icons/folder.png");
	LoadEditorIcon(Icon_Scene, "icons/belt.png");
	LoadEditorIcon(Icon_Image, "icons/image.png");
	LoadEditorIcon(Icon_Mesh, "icons/3d.png");
	LoadEditorIcon(Icon_Code, "icons/code.png");
	LoadEditorIcon(Icon_Header, "icons/header.png");
	LoadEditorIcon(Icon_Audio, "icons/audio.png");
	LoadEditorIcon(Icon_Font, "icons/font.png");
	LoadEditorIcon(Icon_Sky, "icons/sky.png");
	LoadEditorIcon(Icon_Audio_Source, "icons/audio_source.png");
	LoadEditorIcon(Icon_Light, "icons/light.png");
	LoadEditorIcon(Icon_Camera, "icons/camera.png");
	LoadEditorIcon(Icon_Material, "icons/material.png");
	LoadEditorIcon(Icon_Shader, "icons/shader.png");

	Debug::Print("---- Editor UI initiated ----");
	return 0;
}

void EditorUI::Draw()
{
	NewFrame();
	Render();
}

#pragma endregion

#pragma region Update

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
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	uiId = 0;
}

void EditorUI::UpdateUIScale()
{
	ImGuiIO& io = ImGui::GetIO();
	//float y = io.DisplaySize.y;

	SDL_DisplayMode DM;
	int index = SDL_GetWindowDisplayIndex(Window::window);
	//float oldUiScale = uiScale;
	if (index >= 0)
	{
		float dpi = 0;
		float unused;
		int result = SDL_GetDisplayDPI(index, &dpi, &unused, &unused);
		if (result == 0)
		{
			uiScale = dpi / 96.0f;
		}
		else
		{
			SDL_GetCurrentDisplayMode(index, &DM);
			uiScale = DM.h / 1080.0f;
		}
	}

	//if (oldUiScale != uiScale)
	//{
		//io.Fonts->Clear();
	ImGui::GetIO().FontGlobalScale = 0.5f * uiScale;
	//}
}

void EditorUI::LoadEditorIcon(IconName iconName, const std::string& path)
{
	std::shared_ptr<Texture> fileIcon = Texture::MakeTexture();
	//fileIcon->file = FileSystem::MakeFile("icons/text.png");
	fileIcon->file = FileSystem::MakeFile(path.c_str());
	fileIcon->SetWrapMode(Texture::ClampToEdge);
	fileIcon->LoadFileReference();
	icons[iconName] = std::move(fileIcon);
}

/**
* Render the editor's UI
*/
void EditorUI::Render()
{
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
	std::string itemId = "##" + std::to_string(uiId);
	uiId++;
	return itemId;
}

void EditorUI::DrawTextCentered(const std::string& text)
{
	float windowWidth = ImGui::GetWindowSize().x;
	float textWidth = ImGui::CalcTextSize(text.c_str()).x;
	ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
	ImGui::Text(text.c_str());
}

#pragma endregion

#endif