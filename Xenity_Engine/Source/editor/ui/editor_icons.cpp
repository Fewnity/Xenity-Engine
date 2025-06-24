#include "editor_icons.h"
#include "editor_ui.h"

std::vector<std::shared_ptr<Texture>> EditorIcons::s_icons;

const std::vector<std::shared_ptr<Texture>>& EditorIcons::GetIcons()
{
    return s_icons;
}

const std::shared_ptr<Texture>& EditorIcons::GetIcon(IconName iconName)
{
	return s_icons[static_cast<int>(iconName)];
}

void EditorIcons::Init()
{
	s_icons.resize(static_cast<int>(IconName::Icon_Count));

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
	LoadEditorIcon(IconName::Icon_Grid, "icons/grid.png");

	// Platform icons
	LoadEditorIcon(IconName::Icon_Platform_Windows, "icons/platform_windows.png");
	LoadEditorIcon(IconName::Icon_Platform_Linux, "icons/platform_linux.png");
	LoadEditorIcon(IconName::Icon_Platform_PSP, "icons/platform_psp.png");
	LoadEditorIcon(IconName::Icon_Platform_PSVITA, "icons/platform_psvita.png");
	LoadEditorIcon(IconName::Icon_Platform_PS2, "icons/platform_ps2.png");
	LoadEditorIcon(IconName::Icon_Platform_PS3, "icons/platform_ps3.png");
	LoadEditorIcon(IconName::Icon_Platform_PS4, "icons/platform_ps4.png");

	// Other icons
	LoadEditorIcon(IconName::Icon_Question, "icons/question.png");
}

void EditorIcons::LoadEditorIcon(IconName iconName, const std::string& path)
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	std::shared_ptr<Texture> fileIcon = Texture::MakeTexture();
	fileIcon->m_file = FileSystem::MakeFile(path);
	fileIcon->SetWrapMode(WrapMode::ClampToEdge);
	FileReference::LoadOptions loadOptions;
	loadOptions.platform = Application::GetPlatform();
	loadOptions.threaded = true;
	fileIcon->LoadFileReference(loadOptions);

	s_icons[(int)iconName] = std::move(fileIcon);
}