// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "class_registry.h"

#if defined(EDITOR)
#include <editor/ui/menus/menu.h>
#include <editor/ui/menus/project_settings_menu.h>
#include <editor/ui/menus/engine_settings_menu.h>
#include <editor/ui/menus/file_explorer_menu.h>
#include <editor/ui/menus/hierarchy_menu.h>
#include <editor/ui/menus/inspector_menu.h>
#include <editor/ui/menus/main_bar_menu.h>
#include <editor/ui/menus/profiler_menu.h>
#include <editor/ui/menus/game_menu.h>
#include <editor/ui/menus/scene_menu.h>
#include <editor/ui/menus/compiling_menu.h>
#include <editor/ui/menus/select_project_menu.h>
#include <editor/ui/menus/create_project_menu.h>
#include <editor/ui/menus/lighting_menu.h>
#include <editor/ui/menus/create_class_menu.h>
#include <editor/ui/menus/about_menu.h>
#include <editor/ui/menus/console_menu.h>
#include <editor/ui/menus/docker_config_menu.h>
#include <editor/ui/menus/build_settings_menu.h>
#include <editor/ui/menus/engine_asset_manager_menu.h>
#include <editor/ui/menus/engine_debug_menu.h>
std::unordered_map <std::string, std::pair<std::function<std::shared_ptr<Menu>()>, bool>> ClassRegistry::s_nameToMenu;
std::vector<ClassRegistry::MenuClassInfo> ClassRegistry::s_menuClassInfos;
#endif

#include <engine/lighting/lighting.h>
#include <engine/graphics/camera.h>
#include <engine/graphics/ui/canvas.h>
#include <engine/graphics/ui/text_renderer.h>
#include <engine/graphics/ui/text_mesh.h>
#include <engine/graphics/3d_graphics/mesh_renderer.h>
#include <engine/graphics/3d_graphics/lod.h>
#include <engine/graphics/2d_graphics/tile_map.h>
#include <engine/graphics/2d_graphics/sprite_renderer.h>
#include <engine/graphics/2d_graphics/billboard_renderer.h>
#include <engine/graphics/2d_graphics/line_renderer.h>
#include <engine/game_elements/rect_transform.h>
#include <engine/audio/audio_source.h>
#include <engine/test_component.h>
#include <engine/physics/rigidbody.h>
#include <engine/physics/box_collider.h>
#include <engine/physics/sphere_collider.h>
#include <engine/test_component.h>
#include <engine/missing_script.h>
#include <engine/graphics/texture.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/audio/audio_clip.h>
#include <engine/scene_management/scene.h>
#include <engine/graphics/skybox.h>
#include <engine/graphics/ui/font.h>
#include <engine/graphics/shader.h>
#include <engine/graphics/material.h>
#include <engine/graphics/icon.h>
#include <engine/particle_system/particle_system.h>
#include <engine/debug/stack_debug_object.h>

std::unordered_map <std::string, std::pair<std::function<std::shared_ptr<Component>(GameObject&)>, bool>> ClassRegistry::s_nameToComponent;
std::vector<ClassRegistry::FileClassInfo> ClassRegistry::s_fileClassInfos;
std::vector<ClassRegistry::ClassInfo> ClassRegistry::s_classInfos;

std::shared_ptr<Component> ClassRegistry::AddComponentFromName(const std::string& name, GameObject& gameObject)
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	XASSERT(!name.empty(), "[ClassRegistry::AddComponentFromName] name is empty");

	if (s_nameToComponent.find(name) != s_nameToComponent.end()) // Check if the component is in the list
	{
		std::shared_ptr<Component> component = s_nameToComponent[name].first(gameObject);
		return component; // Call the function to add the component to the gameObject
	}
	else
	{
		return nullptr;
	}
}

#if defined (EDITOR)
std::shared_ptr<Menu> ClassRegistry::CreateMenuFromName(const std::string& name)
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	XASSERT(!name.empty(), "[ClassRegistry::AddComponentFromName] name is empty");

	if (s_nameToMenu.find(name) != s_nameToMenu.end()) // Check if the component is in the list
	{
		return s_nameToMenu[name].first(); // Call the function to add the component to the gameObject
	}
	else
	{
		XASSERT(false, "[ClassRegistry::CreateMenuFromName] Cannot create a menu with this name");
		return nullptr;
	}
}
#endif

std::vector<std::string> ClassRegistry::GetComponentNames()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	std::vector<std::string> names;
	for (const auto& kv : s_nameToComponent)
	{
		if(kv.second.second)
			names.push_back(kv.first);
	}
	return names;
}

void ClassRegistry::Reset()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	s_nameToComponent.clear();
	s_classInfos.clear();
}

void ClassRegistry::RegisterEngineComponents()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	// List all Engine components
	REGISTER_COMPONENT(Light);
	REGISTER_COMPONENT(Camera);
	REGISTER_COMPONENT(TextRenderer);
	REGISTER_COMPONENT(Canvas);
	REGISTER_COMPONENT(RectTransform);
	REGISTER_COMPONENT(TextMesh);
	REGISTER_COMPONENT(MeshRenderer);
#if defined(ENABLE_EXPEREMENTAL_FEATURES)
	REGISTER_COMPONENT(Tilemap);
#endif // ENABLE_EXPEREMENTAL_FEATURES
	REGISTER_COMPONENT(SpriteRenderer);
	REGISTER_COMPONENT(BillboardRenderer);
#if defined(ENABLE_EXPEREMENTAL_FEATURES)
	REGISTER_COMPONENT(LineRenderer);
#endif // ENABLE_EXPEREMENTAL_FEATURES
	REGISTER_COMPONENT(AudioSource);
	REGISTER_COMPONENT(ParticleSystem);
	REGISTER_COMPONENT(RigidBody);
	REGISTER_COMPONENT(BoxCollider);
	REGISTER_COMPONENT(SphereCollider);
	REGISTER_COMPONENT(Lod);
#if defined(DEBUG)
	REGISTER_COMPONENT(TestComponent);
#endif
	REGISTER_INVISIBLE_COMPONENT(MissingScript);
}

void ClassRegistry::RegisterEngineFileClasses()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	// List all Engine file classes
	REGISTER_FILE(Texture, FileType::File_Texture);
	REGISTER_FILE(MeshData, FileType::File_Mesh);
	REGISTER_FILE(AudioClip, FileType::File_Audio);
	REGISTER_FILE(Scene, FileType::File_Scene);
	REGISTER_FILE(SkyBox, FileType::File_Skybox);
	REGISTER_FILE(Font, FileType::File_Font);
	REGISTER_FILE(Shader, FileType::File_Shader);
	REGISTER_FILE(Material, FileType::File_Material);
	REGISTER_FILE(Icon, FileType::File_Icon);
}
#if defined (EDITOR)
void ClassRegistry::RegisterMenus()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	REGISTER_MENU(CreateClassMenu);
	REGISTER_MENU(LightingMenu);
	REGISTER_MENU(ProjectSettingsMenu);
	REGISTER_MENU(EngineSettingsMenu);
	REGISTER_MENU(DockerConfigMenu);
	REGISTER_MENU(AboutMenu);
	REGISTER_MENU(BuildSettingsMenu);
	REGISTER_MENU(EngineAssetManagerMenu);
	REGISTER_MENU(EngineDebugMenu);

	REGISTER_MENU(FileExplorerMenu);
	REGISTER_MENU(HierarchyMenu);
	REGISTER_MENU(InspectorMenu);
	REGISTER_MENU(ProfilerMenu);
	REGISTER_MENU(GameMenu);
	REGISTER_MENU(SceneMenu);
	REGISTER_MENU(CompilingMenu);
	REGISTER_MENU(SelectProjectMenu);
	REGISTER_MENU(CreateProjectMenu);
	REGISTER_MENU(ConsoleMenu);
}
#endif