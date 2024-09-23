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
		return s_nameToComponent[name].first(gameObject); // Call the function to add the component to the gameObject
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
	AddComponentClass<Light>("Light");
	AddComponentClass<Camera>("Camera");
	AddComponentClass<TextRenderer>("TextRenderer");
	AddComponentClass<Canvas>("Canvas");
	AddComponentClass<RectTransform>("RectTransform");
	AddComponentClass<TextMesh>("TextMesh");
	AddComponentClass<MeshRenderer>("MeshRenderer");
	AddComponentClass<Tilemap>("Tilemap");
	AddComponentClass<SpriteRenderer>("SpriteRenderer");
	AddComponentClass<BillboardRenderer>("BillboardRenderer");
	AddComponentClass<LineRenderer>("LineRenderer");
	AddComponentClass<AudioSource>("AudioSource");
	AddComponentClass<ParticleSystem>("ParticleSystem");
	AddComponentClass<RigidBody>("RigidBody");
	AddComponentClass<BoxCollider>("BoxCollider");
	AddComponentClass<SphereCollider>("SphereCollider");
	AddComponentClass<Lod>("Lod");
	AddComponentClass<TestComponent>("TestComponent");
	AddComponentClass<MissingScript>("MissingScript", false);
}

void ClassRegistry::RegisterEngineFileClasses()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	// List all Engine file classes
	AddFileClass<Texture>("Texture", FileType::File_Texture);
	AddFileClass<MeshData>("MeshData", FileType::File_Mesh);
	AddFileClass<AudioClip>("AudioClip", FileType::File_Audio);
	AddFileClass<Scene>("Scene", FileType::File_Scene);
	AddFileClass<SkyBox>("SkyBox", FileType::File_Skybox);
	AddFileClass<Font>("Font", FileType::File_Font);
	AddFileClass<Shader>("Shader", FileType::File_Shader);
	AddFileClass<Material>("Material", FileType::File_Material);
	AddFileClass<Icon>("Icon", FileType::File_Icon);
}
#if defined (EDITOR)
void ClassRegistry::RegisterMenus()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	AddMenuClass<CreateClassMenu>("CreateClassMenu");
	AddMenuClass<LightingMenu>("LightingMenu");
	AddMenuClass<ProjectSettingsMenu>("ProjectSettingsMenu");
	AddMenuClass<EngineSettingsMenu>("EngineSettingsMenu");
	AddMenuClass<DockerConfigMenu>("DockerConfigMenu");
	AddMenuClass<AboutMenu>("AboutMenu");
	AddMenuClass<BuildSettingsMenu>("BuildSettingsMenu");
	AddMenuClass<EngineAssetManagerMenu>("EngineAssetManagerMenu");

	AddMenuClass<FileExplorerMenu>("FileExplorerMenu");
	AddMenuClass<HierarchyMenu>("HierarchyMenu");
	AddMenuClass<InspectorMenu>("InspectorMenu");
	AddMenuClass<ProfilerMenu>("ProfilerMenu");
	AddMenuClass<GameMenu>("GameMenu");
	AddMenuClass<SceneMenu>("SceneMenu");
	AddMenuClass<CompilingMenu>("CompilingMenu");
	AddMenuClass<SelectProjectMenu>("SelectProjectMenu");
	AddMenuClass<CreateProjectMenu>("CreateProjectMenu");
	AddMenuClass<ConsoleMenu>("ConsoleMenu");
}
#endif