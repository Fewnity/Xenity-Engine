#include "class_registry.h"
#include <engine/lighting/lighting.h>
#include <engine/graphics/camera.h>
#include <engine/graphics/ui/canvas.h>
#include <engine/graphics/ui/text_renderer_canvas.h>
#include <engine/graphics/ui/text_renderer.h>
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
#include <engine/test_component.h>

#include <engine/graphics/texture.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/audio/audio_clip.h>
#include <engine/scene_management/scene.h>
#include <engine/graphics/skybox.h>
#include <engine/graphics/ui/font.h>
#include <engine/graphics/shader.h>
#include <engine/graphics/material.h>

std::unordered_map <std::string, std::function<std::shared_ptr<Component>(const std::shared_ptr<GameObject>&)>> ClassRegistry::nameToComponent;
std::vector<ClassRegistry::FileClassInfo> ClassRegistry::fileClassInfos;
std::vector<ClassRegistry::ClassInfo> ClassRegistry::classInfos;

std::shared_ptr<Component> ClassRegistry::AddComponentFromName(const std::string& name, const std::shared_ptr<GameObject>& gameObject)
{
	if (name.empty()) 
	{
		Debug::PrintError("[ClassRegistry::AddComponentFromName] Empty component name", true);
		return nullptr;
	}
	else
	{
		if (nameToComponent.find(name) != nameToComponent.end()) // Check if the component is in the list
		{
			return nameToComponent[name](gameObject); // Call the function to add the component to the gameObject
		}
		else
		{
			return nullptr;
		}
	}
}

std::vector<std::string> ClassRegistry::GetComponentNames()
{
	std::vector<std::string> names;
	for (const auto& kv : nameToComponent)
	{
		names.push_back(kv.first);
	}
	return names;
}

void ClassRegistry::Reset()
{
	nameToComponent.clear();
	classInfos.clear();
}

void ClassRegistry::RegisterEngineComponents()
{
	// List all Engine components
	AddComponentClass<Light>("Light");
	AddComponentClass<Camera>("Camera");
	AddComponentClass<TextRendererCanvas>("TextRendererCanvas");
	AddComponentClass<Canvas>("Canvas");
	AddComponentClass<RectTransform>("RectTransform");
	AddComponentClass<TextRenderer>("TextRenderer");
	AddComponentClass<MeshRenderer>("MeshRenderer");
	AddComponentClass<Tilemap>("Tilemap");
	AddComponentClass<SpriteRenderer>("SpriteRenderer");
	AddComponentClass<BillboardRenderer>("BillboardRenderer");
	AddComponentClass<LineRenderer>("LineRenderer");
	AddComponentClass<AudioSource>("AudioSource");
	AddComponentClass<TestComponent>("TestComponent");
	AddComponentClass<RigidBody>("RigidBody");
	AddComponentClass<BoxCollider>("BoxCollider");
	AddComponentClass<Lod>("Lod");
}


void ClassRegistry::RegisterEngineFileClasses()
{
	// List all Engine file classes
	AddFileClass<Texture>("Texture", FileType::File_Texture);
	AddFileClass<MeshData>("MeshData", FileType::File_Mesh);
	AddFileClass<AudioClip>("AudioClip", FileType::File_Audio);
	AddFileClass<Scene>("Scene", FileType::File_Scene);
	AddFileClass<SkyBox>("SkyBox", FileType::File_Skybox);
	AddFileClass<Font>("Font", FileType::File_Font);
	AddFileClass<Shader>("Shader", FileType::File_Shader);
	AddFileClass<Material>("Material", FileType::File_Material);
}