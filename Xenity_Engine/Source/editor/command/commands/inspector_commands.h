#pragma once

#include "../command.h"
#include <memory>
#include "../../../engine/scene_management/scene_manager.h"
#include "../../../engine/game_elements/gameobject.h"

template<typename U, typename T>
class InspectorChangeValueCommand : public Command
{
public:
	InspectorChangeValueCommand() = delete;
	InspectorChangeValueCommand(std::weak_ptr<U> target, T* valuePtr, T newValue, T lastValue);
	void Execute();
	void Undo();
private:
	std::weak_ptr<U> target;
	T* valuePtr;
	T newValue;
	T lastValue;
};

template<typename U, typename T>
inline InspectorChangeValueCommand<U, T>::InspectorChangeValueCommand(std::weak_ptr<U> target, T* valuePtr, T newValue, T lastValue)
{
	this->target = target;
	this->valuePtr = valuePtr;
	this->newValue = newValue;
	this->lastValue = lastValue;
}

template<typename U, typename T>
inline void InspectorChangeValueCommand<U, T>::Execute()
{
	*valuePtr = newValue;
	SceneManager::SetSceneModified(true);
}

template<typename U, typename T>
inline void InspectorChangeValueCommand<U, T>::Undo()
{
	if (target.lock()) 
	{
		*valuePtr = lastValue;
		SceneManager::SetSceneModified(true);
	}
}

class InspectorGameObjectSetActiveCommand : public Command
{
public:
	InspectorGameObjectSetActiveCommand() = delete;
	InspectorGameObjectSetActiveCommand(std::weak_ptr<GameObject> target, bool newValue, bool lastValue);
	void Execute();
	void Undo();
private:
	std::weak_ptr<GameObject> target;
	bool newValue;
	bool lastValue;
};

inline InspectorGameObjectSetActiveCommand::InspectorGameObjectSetActiveCommand(std::weak_ptr<GameObject> target, bool newValue, bool lastValue)
{
	this->target = target;
	this->newValue = newValue;
	this->lastValue = lastValue;
}

inline void InspectorGameObjectSetActiveCommand::Execute()
{
	target.lock()->SetActive(newValue);
	SceneManager::SetSceneModified(true);
}

inline void InspectorGameObjectSetActiveCommand::Undo()
{
	if (target.lock())
	{
		target.lock()->SetActive(lastValue);
		SceneManager::SetSceneModified(true);
	}
}
