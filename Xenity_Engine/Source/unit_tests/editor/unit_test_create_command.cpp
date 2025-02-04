// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
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

TestResult CreateEmptyGameObjectCommandTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	std::vector<std::weak_ptr<GameObject>> targets;
	InspectorCreateGameObjectCommand command = InspectorCreateGameObjectCommand(targets, CreateGameObjectMode::CreateEmpty);

	EXPECT_EQUALS(command.createdGameObjects.size(), 0, "InspectorCreateGameObjectCommand not correctly initialized");

	command.Execute();

	EXPECT_EQUALS(command.createdGameObjects.size(), 1, "Failed to create a GameObject");

	const uint64_t createdGameObjectId = command.createdGameObjects[0];
	{
		std::shared_ptr<GameObject> gameObject = FindGameObjectById(createdGameObjectId);
		EXPECT_NOT_NULL(gameObject, "Failed to find the GameObject after the creation");
	}
	command.Undo();

	GameplayManager::RemoveDestroyedGameObjects();

	{
		std::shared_ptr<GameObject> gameObject = FindGameObjectById(createdGameObjectId);
		EXPECT_NULL(gameObject, "Failed to undo the command");
	}

	command.Redo();

	{
		std::shared_ptr<GameObject> gameObject = FindGameObjectById(createdGameObjectId);
		EXPECT_NOT_NULL(gameObject, "Failed to find the GameObject after the re-creation");
	}

	command.Undo();

	GameplayManager::RemoveDestroyedGameObjects();

	{
		std::shared_ptr<GameObject> gameObject = FindGameObjectById(createdGameObjectId);
		EXPECT_NULL(gameObject, "Failed to undo the command 2");
	}

	command.Redo();

	{
		std::shared_ptr<GameObject> gameObject = FindGameObjectById(createdGameObjectId);
		EXPECT_NOT_NULL(gameObject, "Failed to find the GameObject after the re-creation 2");
	}

	command.Undo();

	SceneManager::SetSceneModified(false);

	END_TEST();
}

TestResult CreateChildGameObjectCommandTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	std::vector<std::weak_ptr<GameObject>> targets;
	std::shared_ptr<GameObject> parent = CreateGameObject();
	std::shared_ptr<GameObject> parent2 = CreateGameObject();
	targets.push_back(parent);
	targets.push_back(parent2);

	InspectorCreateGameObjectCommand command = InspectorCreateGameObjectCommand(targets, CreateGameObjectMode::CreateChild);

	EXPECT_EQUALS(command.createdGameObjects.size(), 0, "InspectorCreateGameObjectCommand not correctly initialized");

	command.Execute();

	EXPECT_EQUALS(command.createdGameObjects.size(), 2, "Failed to create a GameObject");

	const uint64_t createdGameObjectId0 = command.createdGameObjects[0];
	const uint64_t createdGameObjectId1 = command.createdGameObjects[1];

	{
		std::shared_ptr<GameObject> gameObject0 = FindGameObjectById(createdGameObjectId0);
		EXPECT_NOT_NULL(gameObject0, "Failed to find the GameObject after the creation");

		std::shared_ptr<GameObject> gameObject1 = FindGameObjectById(createdGameObjectId1);
		EXPECT_NOT_NULL(gameObject1, "Failed to find the GameObject after the creation");
	}

	command.Undo();
	GameplayManager::RemoveDestroyedGameObjects();

	{
		std::shared_ptr<GameObject> gameObject0 = FindGameObjectById(createdGameObjectId0);
		EXPECT_NULL(gameObject0, "Failed to undo the command");

		std::shared_ptr<GameObject> gameObject1 = FindGameObjectById(createdGameObjectId1);
		EXPECT_NULL(gameObject1, "Failed to undo the command");
	}

	Destroy(parent);
	Destroy(parent2);
	GameplayManager::RemoveDestroyedGameObjects();
	SceneManager::SetSceneModified(false);

	END_TEST();
}

TestResult CreateParentGameObjectCommandTest::Start(std::string& errorOut)
{
	BEGIN_TEST();
	END_TEST();
}

#endif