#include "class_registry.h"
#include <engine/lighting/lighting.h>
#include <engine/graphics/camera.h>
#include <engine/graphics/ui/text_renderer_canvas.h>
#include <engine/graphics/ui/text_renderer.h>
#include <engine/graphics/3d_graphics/mesh_renderer.h>
#include <engine/graphics/2d_graphics/tile_map.h>
#include <engine/graphics/2d_graphics/sprite_renderer.h>
#include <engine/graphics/2d_graphics/billboard_renderer.h>
#include <engine/graphics/2d_graphics/line_renderer.h>
#include <engine/audio/audio_source.h>
#include <engine/test_component.h>
#include <engine/physics/rigidbody.h>
#include <engine/physics/box_collider.h>
#include <engine/test_component.h>

std::unordered_map <std::string, std::function<std::shared_ptr<Component>(const std::shared_ptr<GameObject>&)>> ClassRegistry::nameToComponent;

std::shared_ptr<Component> ClassRegistry::AddComponentFromName(const std::string& name, const std::shared_ptr<GameObject>& gameObject)
{
	if (name.empty()) 
	{
		Debug::PrintError("[ClassRegistry::AddComponentFromName] Empty component name");
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
	for (auto& kv : nameToComponent)
	{
		names.push_back(kv.first);
	}
	return names;
}

void ClassRegistry::Reset()
{
	nameToComponent.clear();
}

void ClassRegistry::RegisterEngineComponents()
{
	// List all Engine components
	AddComponentClass<Light>("Light");
	AddComponentClass<Camera>("Camera");
	AddComponentClass<TextRendererCanvas>("TextRendererCanvas");
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
}