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

TestResult AddComponentCommandTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	std::shared_ptr<GameObject> newGameObject = CreateGameObject();

	EXPECT_EQUALS(newGameObject->GetComponentCount(), 0, "Component is not empty by default");

	//----------------------------------------------------------------------------  Simple test with one component
	{
		InspectorAddComponentCommand addComponentCommand(*newGameObject, "Light");
		addComponentCommand.Execute();
		const uint64_t lightId = addComponentCommand.componentId;
		
		EXPECT_NOT_NULL(newGameObject->GetComponent<Light>(), "Failed to add Light component");

		EXPECT_TRUE(SceneManager::GetSceneModified(), "The scene is not dirty");

		addComponentCommand.Undo();

		EXPECT_EQUALS(newGameObject->GetComponentCount(), 0, "Light component has not been removed");

		addComponentCommand.Redo();

		EXPECT_NOT_NULL(newGameObject->GetComponent<Light>(), "Failed to re add Light component");

		EXPECT_EQUALS(newGameObject->GetComponent<Light>()->GetUniqueId(), lightId, "Re added light has wrong unique id");
		
		addComponentCommand.Undo();

		EXPECT_EQUALS(newGameObject->GetComponentCount(), 0, "Light component has not been removed");
	}

	//----------------------------------------------------------------------------  Advanced test with three components
	{
		InspectorAddComponentCommand addComponentCommandLight(*newGameObject, "Light");
		addComponentCommandLight.Execute();

		EXPECT_NOT_NULL(newGameObject->GetComponent<Light>(), "Failed to add Light component");

		InspectorAddComponentCommand addComponentCommandAudioSource(*newGameObject, "AudioSource");
		addComponentCommandAudioSource.Execute();
		const uint64_t audioSourceId = addComponentCommandAudioSource.componentId;

		EXPECT_NOT_NULL(newGameObject->GetComponent<AudioSource>(), "Failed to add AudioSource component");

		InspectorAddComponentCommand addComponentCommandAudioSource2(*newGameObject, "AudioSource");
		addComponentCommandAudioSource2.Execute();

		EXPECT_EQUALS(newGameObject->GetComponentCount(), 3, "Failed to add AudioSource component");
		EXPECT_NOT_EQUALS(newGameObject->GetComponents<AudioSource>()[0], newGameObject->GetComponents<AudioSource>()[1], "Both AudioSource are the same");

		addComponentCommandAudioSource.Undo();

		EXPECT_EQUALS(newGameObject->GetComponentCount(), 2, "AudioSource component has not been removed");
		EXPECT_NOT_EQUALS(newGameObject->GetComponent<AudioSource>()->GetUniqueId(), audioSourceId, "The wrong AudioSource component has removed");

		addComponentCommandAudioSource2.Undo();
		EXPECT_EQUALS(newGameObject->GetComponentCount(), 1, "AudioSource component has not been removed");

		addComponentCommandLight.Undo();
		EXPECT_EQUALS(newGameObject->GetComponentCount(), 0, "Light component has not been removed");
	}

	Destroy(newGameObject);
	GameplayManager::RemoveDestroyedGameObjects();
	GameplayManager::RemoveDestroyedComponents();
	newGameObject.reset();
	
	SceneManager::SetSceneModified(false);

	END_TEST();
}

#endif