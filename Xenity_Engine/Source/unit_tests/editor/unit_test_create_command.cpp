// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(EDITOR)

#include "../unit_test_manager.h"
#include <engine/debug/debug.h>
#include <engine/game_elements/gameobject.h>
#include <editor/command/commands/create.h>
#include <engine/game_elements/gameplay_manager.h>
#include <engine/lighting/lighting.h>
#include <engine/audio/audio_source.h>

bool AddComponentCommandTest::Start(std::string& errorOut)
{
	bool result = true;

	// Test Vector2
	/*Vector2 v2A = Vector2(1, 4.5f);
	Vector2 v2B = Vector2(3, 1);
	if (!Compare(v2A + v2B, Vector2(4, 5.5f)))
	{
		errorOut += "Bad Vector2 addition\n";
		result = false;
	}*/


	std::shared_ptr<GameObject> newGameObject = CreateGameObject();

	if (!Compare(newGameObject->GetComponentCount(), 0))
	{
		errorOut += "Component is not empty by default\n";
		result = false;
	}

	//----------------------------------------------------------------------------  Simple test with one component
	{
		InspectorAddComponentCommand addComponentCommand(*newGameObject, "Light");
		addComponentCommand.Execute();
		uint64_t lightId = addComponentCommand.componentId;
		if (!newGameObject->GetComponent<Light>())
		{
			errorOut += "Failed to add Light component\n";
			result = false;
		}

		if (!Compare(SceneManager::GetSceneModified(), true))
		{
			errorOut += "The scene is not dirty\n";
			result = false;
		}

		addComponentCommand.Undo();

		if (!Compare(newGameObject->GetComponentCount(), 0))
		{
			errorOut += "Light component has not been removed\n";
			result = false;
		}

		addComponentCommand.Redo();

		if (!newGameObject->GetComponent<Light>())
		{
			errorOut += "Failed to re add Light component\n";
			result = false;
		}

		if (!Compare(newGameObject->GetComponent<Light>()->GetUniqueId(), lightId))
		{
			errorOut += "Re added light has wrong unique id\n";
			result = false;
		}
		
		addComponentCommand.Undo();

		if (!Compare(newGameObject->GetComponentCount(), 0))
		{
			errorOut += "Light component has not been removed\n";
			result = false;
		}
	}

	//----------------------------------------------------------------------------  Advanced test with three components
	{
		InspectorAddComponentCommand addComponentCommandLight(*newGameObject, "Light");
		addComponentCommandLight.Execute();
		if (!newGameObject->GetComponent<Light>())
		{
			errorOut += "Failed to add Light component\n";
			result = false;
		}

		InspectorAddComponentCommand addComponentCommandAudioSource(*newGameObject, "AudioSource");
		addComponentCommandAudioSource.Execute();
		const uint64_t audioSourceId = addComponentCommandAudioSource.componentId;
		if (!newGameObject->GetComponent<AudioSource>())
		{
			errorOut += "Failed to add AudioSource component\n";
			result = false;
		}

		InspectorAddComponentCommand addComponentCommandAudioSource2(*newGameObject, "AudioSource");
		addComponentCommandAudioSource2.Execute();


		if (!Compare(newGameObject->GetComponentCount(), 3))
		{
			errorOut += "Failed to add AudioSource component\n";
			result = false;
		}

		if (newGameObject->GetComponents<AudioSource>()[0] == newGameObject->GetComponents<AudioSource>()[1]) 
		{
			errorOut += "Both AudioSource are the same\n";
			result = false;
		}

		addComponentCommandAudioSource.Undo();

		if (!Compare(newGameObject->GetComponentCount(), 2))
		{
			errorOut += "AudioSource component has not been removed\n";
			result = false;
		}

		if (newGameObject->GetComponent<AudioSource>()->GetUniqueId() == audioSourceId)
		{
			errorOut += "The wrong AudioSource component has removed\n";
			result = false;
		}

		addComponentCommandAudioSource2.Undo();
		
		if (!Compare(newGameObject->GetComponentCount(), 1))
		{
			errorOut += "AudioSource component has not been removed\n";
			result = false;
		}

		addComponentCommandLight.Undo();

		if (!Compare(newGameObject->GetComponentCount(), 0))
		{
			errorOut += "Light component has not been removed\n";
			result = false;
		}
	}

	Destroy(newGameObject);
	GameplayManager::RemoveDestroyedGameObjects();
	GameplayManager::RemoveDestroyedComponents();
	newGameObject.reset();
	
	SceneManager::SetSceneModified(false);

	return result;
}

#endif