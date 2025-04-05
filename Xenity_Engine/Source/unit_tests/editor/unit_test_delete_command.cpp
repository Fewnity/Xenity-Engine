// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(EDITOR)

#include "../unit_test_manager.h"

#include <engine/debug/debug.h>
#include <engine/game_elements/gameobject.h>
#include <editor/command/commands/delete.h>
#include <engine/game_elements/gameplay_manager.h>
#include <engine/lighting/lighting.h>
#include <engine/audio/audio_source.h>

TestResult DeleteComponentCommandTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	std::shared_ptr<GameObject> newGameObject = CreateGameObject();

	EXPECT_EQUALS(newGameObject->GetComponentCount(), 0, "Component is not empty by default");

	std::shared_ptr<AudioSource> audioSource = newGameObject->AddComponent<AudioSource>();
	audioSource->SetIsEnabled(false);

	EXPECT_EQUALS(newGameObject->GetComponentCount(), 1, "Component has been not added to the gameobject");

	//----------------------------------------------------------------------------  Simple test with one component
	{
		InspectorDeleteComponentCommand deleteComponentCommand(*audioSource);
		uint64_t lightId = 0;

		deleteComponentCommand.Execute();
		{
			EXPECT_EQUALS(newGameObject->GetComponentCount(), 0, "Component has been not removed from the gameobject");

			lightId = deleteComponentCommand.GetComponentId();

			EXPECT_NULL(newGameObject->GetComponent<AudioSource>(), "Failed to remove Light component");

			EXPECT_TRUE(SceneManager::GetSceneModified(), "The scene is not dirty");
		}
		audioSource.reset();


		deleteComponentCommand.Undo();
		{
			EXPECT_EQUALS(newGameObject->GetComponentCount(), 1, "Component has been not readded from the gameobject");

			audioSource = newGameObject->GetComponent<AudioSource>();

			EXPECT_NOT_NULL(audioSource, "Failed to readd Light component");

			EXPECT_EQUALS(audioSource->GetUniqueId(), lightId, "Recreated component has the wrong id");

			EXPECT_TRUE(SceneManager::GetSceneModified(), "The scene is not dirty");

			EXPECT_FALSE(audioSource->IsEnabled(), "Component should be disabled");
		}
		audioSource.reset();

		deleteComponentCommand.Redo();
		{
			EXPECT_EQUALS(newGameObject->GetComponentCount(), 0, "Component has been not removed from the gameobject");

			EXPECT_NULL(newGameObject->GetComponent<AudioSource>(), "Failed to remove Light component");

			EXPECT_TRUE(SceneManager::GetSceneModified(), "The scene is not dirty");
		}

		deleteComponentCommand.Undo();
		{
			audioSource.reset();

			EXPECT_EQUALS(newGameObject->GetComponentCount(), 1, "Component has been not readded from the gameobject");

			audioSource = newGameObject->GetComponent<AudioSource>();

			EXPECT_NOT_NULL(audioSource, "Failed to readd Light component");

			EXPECT_EQUALS(audioSource->GetUniqueId(), lightId, "Recreated component has the wrong id");

			EXPECT_TRUE(SceneManager::GetSceneModified(), "The scene is not dirty");

			EXPECT_FALSE(audioSource->IsEnabled(), "Component should be disabled");
		}
	}

	Destroy(newGameObject);
	GameplayManager::RemoveDestroyedGameObjects();
	GameplayManager::RemoveDestroyedComponents();
	newGameObject.reset();

	SceneManager::SetSceneModified(false);

	END_TEST();
}

TestResult DeleteGameObjectCommandTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	std::shared_ptr<GameObject> newGameObject = CreateGameObject("Test");

	EXPECT_EQUALS(newGameObject->GetComponentCount(), 0, "Component is not empty by default");

	std::shared_ptr<AudioSource> audioSource = newGameObject->AddComponent<AudioSource>();

	EXPECT_EQUALS(newGameObject->GetComponentCount(), 1, "Component has been not added to the gameobject");

	//----------------------------------------------------------------------------  Simple test with one component
	{
		std::weak_ptr<GameObject> weakGameObject = newGameObject;
		InspectorDeleteGameObjectCommand deleteGameObjectCommand(weakGameObject);
		const uint64_t gameobjectId = newGameObject->GetUniqueId();
		const uint64_t audioSourceId = audioSource->GetUniqueId();

		newGameObject.reset();
		audioSource.reset();
		deleteGameObjectCommand.Execute();
		{
			GameplayManager::RemoveDestroyedGameObjects();
			GameplayManager::RemoveDestroyedComponents();

			newGameObject = FindGameObjectById(gameobjectId);

			EXPECT_NULL(newGameObject, "Failed to execute the command");

			EXPECT_TRUE(SceneManager::GetSceneModified(), "The scene is not dirty");
		}


		deleteGameObjectCommand.Undo();
		{
			newGameObject = FindGameObjectById(gameobjectId);

			EXPECT_NOT_NULL(newGameObject, "Failed to undo the command");
			EXPECT_EQUALS(newGameObject->GetName(), "Test", "Recreated GameObject has the wrong name");

			audioSource = newGameObject->GetComponent<AudioSource>();
			EXPECT_NOT_NULL(audioSource, "Failed to recreate gameobject's component");
			EXPECT_EQUALS(audioSource->GetUniqueId(), audioSourceId, "Recreated component has the wrong id");

			EXPECT_TRUE(SceneManager::GetSceneModified(), "The scene is not dirty");
		}

		deleteGameObjectCommand.Redo();
		{
			GameplayManager::RemoveDestroyedGameObjects();
			GameplayManager::RemoveDestroyedComponents();

			newGameObject = FindGameObjectById(gameobjectId);

			EXPECT_NULL(newGameObject, "Failed to execute the command");

			EXPECT_TRUE(SceneManager::GetSceneModified(), "The scene is not dirty");
		}

		deleteGameObjectCommand.Undo();
		{
			newGameObject = FindGameObjectById(gameobjectId);

			EXPECT_NOT_NULL(newGameObject, "Failed to undo the command");
			EXPECT_EQUALS(newGameObject->GetName(), "Test", "Recreated GameObject has the wrong name");

			audioSource = newGameObject->GetComponent<AudioSource>();
			EXPECT_NOT_NULL(audioSource, "Failed to recreate gameobject's component");
			EXPECT_EQUALS(audioSource->GetUniqueId(), audioSourceId, "Recreated component has the wrong id");

			EXPECT_TRUE(SceneManager::GetSceneModified(), "The scene is not dirty");
		}
	}

	Destroy(newGameObject);
	GameplayManager::RemoveDestroyedGameObjects();
	GameplayManager::RemoveDestroyedComponents();
	newGameObject.reset();

	SceneManager::SetSceneModified(false);

	END_TEST();
}

#endif