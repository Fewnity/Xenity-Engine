#pragma once

#include <vector>
#include <memory>
#include <string>

class Texture;

enum class IconName
{
	Icon_Folder,
	Icon_File,
	Icon_Scene,
	Icon_Image,
	Icon_Mesh,
	Icon_Code,
	Icon_Header,
	Icon_Audio,
	Icon_Sky,
	Icon_Font,
	Icon_Audio_Source,
	Icon_Point_Light,
	Icon_Sun_Light,
	Icon_Spot_Light,
	Icon_Camera,
	Icon_Material,
	Icon_Shader,
	Icon_Play,
	Icon_Pause,
	Icon_Stop,
	Icon_Camera_Move,
	Icon_Move,
	Icon_Rotate,
	Icon_Scale,
	Icon_Grid,
	Icon_Question,
	Icon_Platform_Windows,
	Icon_Platform_Linux,
	Icon_Platform_PSP,
	Icon_Platform_PSVITA,
	Icon_Platform_PS2,
	Icon_Platform_PS3,
	Icon_Platform_PS4,
	Icon_Count, // Last item
};

/*
* @brief Class to manage editor icons
*/
class EditorIcons
{
public:
	/**
	* @brief Get the list of all editor icons
	*/
	static const std::vector<std::shared_ptr<Texture>>& GetIcons();
	
	/**
	* @brief Get an icon by its name enum
	*/
	static const std::shared_ptr<Texture>& GetIcon(IconName iconName);

private:
	friend class EditorUI;

	/**
	* @brief Load all editor icons
	*/
	static void Init();

	/**
	* @brief Load editor icons
	*/
	static void LoadEditorIcon(IconName iconName, const std::string& path);

	static std::vector<std::shared_ptr<Texture>> icons;
};

