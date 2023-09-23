#if defined(EDITOR)

#include "editor_ui.h"
#include "../editor.h"
#include "../../xenity.h"
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_stdlib.h>
#include "../../engine/tools/shape_spawner.h"
#include <variant>
#include <imgui/imgui_internal.h>
#include "../../engine/asset_management/project_manager.h"
#include <Windows.h>
//#include <Commdlg.h>
#include <ShObjIdl.h>
#include "../../engine/graphics/skybox.h"

int EditorUI::uiId = 0;
bool EditorUI::showProfiler = true;
bool EditorUI::showEditor = true;
bool EditorUI::showEngineSettings = false;
bool EditorUI::showProjectsSettings = false;
bool EditorUI::showLightingSettings = false;

std::vector<std::shared_ptr<Texture>>  EditorUI::icons;

float EditorUI::uiScale = 1;

#pragma region Initialisation

void EditorUI::Init()
{
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigWindowsMoveFromTitleBarOnly = true;
	io.Fonts->AddFontFromFileTTF("Roboto Regular.ttf", 30);

	for (int i = 0; i < Icon_Count; i++)
	{
		icons.push_back(std::shared_ptr<Texture>());
	}

	std::shared_ptr<Texture> fileIcon = Texture::MakeTexture("icons/text.png", true);
	fileIcon->file = FileSystem::MakeFile("icons/text.png");
	fileIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_File] = fileIcon;

	std::shared_ptr<Texture> folderIcon = Texture::MakeTexture("icons/folder.png", true);
	folderIcon->file = FileSystem::MakeFile("icons/folder.png");
	folderIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Folder] = folderIcon;

	std::shared_ptr<Texture> sceneIcon = Texture::MakeTexture("icons/belt.png", true);
	sceneIcon->file = FileSystem::MakeFile("icons/belt.png");
	sceneIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Scene] = sceneIcon;

	std::shared_ptr<Texture> imageIcon = Texture::MakeTexture("icons/image.png", true);
	imageIcon->file = FileSystem::MakeFile("icons/image.png");
	imageIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Image] = imageIcon;

	std::shared_ptr<Texture> meshIcon = Texture::MakeTexture("icons/3d.png", true);
	meshIcon->file = FileSystem::MakeFile("icons/3d.png");
	meshIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Mesh] = meshIcon;

	std::shared_ptr<Texture> codeIcon = Texture::MakeTexture("icons/code.png", true);
	codeIcon->file = FileSystem::MakeFile("icons/code.png");
	codeIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Code] = codeIcon;

	std::shared_ptr<Texture> headerIcon = Texture::MakeTexture("icons/header.png", true);
	headerIcon->file = FileSystem::MakeFile("icons/header.png");
	headerIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Header] = headerIcon;

	std::shared_ptr<Texture> audioIcon = Texture::MakeTexture("icons/audio.png", true);
	audioIcon->file = FileSystem::MakeFile("icons/audio.png");
	audioIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Audio] = audioIcon;

	std::shared_ptr<Texture> fontIcon = Texture::MakeTexture("icons/font.png", true);
	fontIcon->file = FileSystem::MakeFile("icons/font.png");
	fontIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Font] = fontIcon;

	std::shared_ptr<Texture> skyIcon = Texture::MakeTexture("icons/sky.png", true);
	skyIcon->file = FileSystem::MakeFile("icons/sky.png");
	skyIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Sky] = skyIcon;

	std::shared_ptr<Texture> audioSourceIcon = Texture::MakeTexture("icons/audio_source.png", true);
	audioSourceIcon->file = FileSystem::MakeFile("icons/audio_source.png");
	audioSourceIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Audio_Source] = audioSourceIcon;

	std::shared_ptr<Texture> lightIcon = Texture::MakeTexture("icons/light.png", true);
	lightIcon->file = FileSystem::MakeFile("icons/light.png");
	lightIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Light] = lightIcon;

	std::shared_ptr<Texture> cameraIcon = Texture::MakeTexture("icons/camera.png", true);
	cameraIcon->file = FileSystem::MakeFile("icons/camera.png");
	cameraIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Camera] = cameraIcon;

	std::shared_ptr<Texture> materialIcon = Texture::MakeTexture("icons/material.png", true);
	materialIcon->file = FileSystem::MakeFile("icons/material.png");
	materialIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Material] = materialIcon;

	std::shared_ptr<Texture> shaderIcon = Texture::MakeTexture("icons/shader.png", true);
	shaderIcon->file = FileSystem::MakeFile("icons/shader.png");
	shaderIcon->SetWrapMode(Texture::ClampToEdge);
	icons[Icon_Shader] = shaderIcon;

	for (int i = 0; i < Icon_Count; i++)
	{
		icons[i]->LoadFileReference();
	}

	Debug::Print("---- Editor UI initiated ----");
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
	int nameSize = variableName.size();
	bool addSpace = false;
	for (int i = 1; i < nameSize - 1; i++)
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
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
}

void EditorUI::UpdateUIScale()
{
	ImGuiIO& io = ImGui::GetIO();
	//float y = io.DisplaySize.y;

	SDL_DisplayMode DM;
	int index = SDL_GetWindowDisplayIndex(Window::window);
	float oldUiScale = uiScale;
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

void EditorUI::SetRoundedCorner(int value)
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

bool EditorUI::DrawReflection(Reflection& reflection)
{
	auto t = reflection.GetReflection();
	return DrawMap(t);
}

void EditorUI::DrawTextCentered(std::string text)
{
	float windowWidth = ImGui::GetWindowSize().x;
	float textWidth = ImGui::CalcTextSize(text.c_str()).x;
	ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
	ImGui::Text(text.c_str());
}

#pragma endregion

#endif