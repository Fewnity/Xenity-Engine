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

	std::shared_ptr<Texture> fileIcon = Texture::MakeTexture();
	fileIcon->file = FileSystem::MakeFile("icons/text.png");
	fileIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_File] = std::move(fileIcon);

	std::shared_ptr<Texture> folderIcon = Texture::MakeTexture();
	folderIcon->file = FileSystem::MakeFile("icons/folder.png");
	folderIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Folder] = std::move(folderIcon);

	std::shared_ptr<Texture> sceneIcon = Texture::MakeTexture();
	sceneIcon->file = FileSystem::MakeFile("icons/belt.png");
	sceneIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Scene] = std::move(sceneIcon);

	std::shared_ptr<Texture> imageIcon = Texture::MakeTexture();
	imageIcon->file = FileSystem::MakeFile("icons/image.png");
	imageIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Image] = std::move(imageIcon);

	std::shared_ptr<Texture> meshIcon = Texture::MakeTexture();
	meshIcon->file = FileSystem::MakeFile("icons/3d.png");
	meshIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Mesh] = std::move(meshIcon);

	std::shared_ptr<Texture> codeIcon = Texture::MakeTexture();
	codeIcon->file = FileSystem::MakeFile("icons/code.png");
	codeIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Code] = std::move(codeIcon);

	std::shared_ptr<Texture> headerIcon = Texture::MakeTexture();
	headerIcon->file = FileSystem::MakeFile("icons/header.png");
	headerIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Header] = std::move(headerIcon);

	std::shared_ptr<Texture> audioIcon = Texture::MakeTexture();
	audioIcon->file = FileSystem::MakeFile("icons/audio.png");
	audioIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Audio] = std::move(audioIcon);

	std::shared_ptr<Texture> fontIcon = Texture::MakeTexture();
	fontIcon->file = FileSystem::MakeFile("icons/font.png");
	fontIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Font] = std::move(fontIcon);

	std::shared_ptr<Texture> skyIcon = Texture::MakeTexture();
	skyIcon->file = FileSystem::MakeFile("icons/sky.png");
	skyIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Sky] = std::move(skyIcon);

	std::shared_ptr<Texture> audioSourceIcon = Texture::MakeTexture();
	audioSourceIcon->file = FileSystem::MakeFile("icons/audio_source.png");
	audioSourceIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Audio_Source] = std::move(audioSourceIcon);

	std::shared_ptr<Texture> lightIcon = Texture::MakeTexture();
	lightIcon->file = FileSystem::MakeFile("icons/light.png");
	lightIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Light] = std::move(lightIcon);

	std::shared_ptr<Texture> cameraIcon = Texture::MakeTexture();
	cameraIcon->file = FileSystem::MakeFile("icons/camera.png");
	cameraIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Camera] = std::move(cameraIcon);

	std::shared_ptr<Texture> materialIcon = Texture::MakeTexture();
	materialIcon->file = FileSystem::MakeFile("icons/material.png");
	materialIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Material] = std::move(materialIcon);

	std::shared_ptr<Texture> shaderIcon = Texture::MakeTexture();
	shaderIcon->file = FileSystem::MakeFile("icons/shader.png");
	shaderIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Shader] = std::move(shaderIcon);

	for (int i = 0; i < Icon_Count; i++)
	{
		icons[i]->LoadFileReference();
	}

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