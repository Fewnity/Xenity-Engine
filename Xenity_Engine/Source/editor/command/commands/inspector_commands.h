#pragma once

#include "../command.h"
#include <memory>
#include "../../../engine/scene_management/scene_manager.h"
#include "../../../engine/game_elements/gameobject.h"
#include "../../editor.h"
#include "../../../engine/engine.h"
#include <json.hpp>
#include "../../../engine/reflection/reflection_utils.h"
#include "../../../engine/class_registry/class_registry.h"

//using json = nlohmann::json;

//----------------------------------------------------------------------------

template<typename U, typename T>
class InspectorChangeValueCommand : public Command
{
public:
	InspectorChangeValueCommand() = delete;
	InspectorChangeValueCommand(std::weak_ptr<U> target, T* valuePtr, T newValue, T lastValue);
	void Execute() override;
	void Undo() override;
	void Redo() override;
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

template<typename U, typename T>
inline void InspectorChangeValueCommand<U, T>::Redo()
{
	if (target.lock())
	{
		*valuePtr = newValue;
		SceneManager::SetSceneModified(true);
	}
}

//----------------------------------------------------------------------------

class InspectorGameObjectSetActiveCommand : public Command
{
public:
	InspectorGameObjectSetActiveCommand() = delete;
	InspectorGameObjectSetActiveCommand(std::weak_ptr<GameObject> target, bool newValue, bool lastValue);
	void Execute() override;
	void Undo() override;
	void Redo() override;
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

inline void InspectorGameObjectSetActiveCommand::Redo()
{
	if (target.lock())
	{
		target.lock()->SetActive(newValue);
		SceneManager::SetSceneModified(true);
	}
}


//----------------------------------------------------------------------------

class InspectorTransformSetLocalPositionCommand : public Command
{
public:
	InspectorTransformSetLocalPositionCommand() = delete;
	InspectorTransformSetLocalPositionCommand(std::weak_ptr<Transform> target, Vector3 newValue, Vector3 lastValue);
	void Execute() override;
	void Undo() override;
	void Redo() override;
private:
	std::weak_ptr<Transform> target;
	Vector3 newValue;
	Vector3 lastValue;
};

inline InspectorTransformSetLocalPositionCommand::InspectorTransformSetLocalPositionCommand(std::weak_ptr<Transform> target, Vector3 newValue, Vector3 lastValue)
{
	this->target = target;
	this->newValue = newValue;
	this->lastValue = lastValue;
}

inline void InspectorTransformSetLocalPositionCommand::Execute()
{
	target.lock()->SetLocalPosition(newValue);
	SceneManager::SetSceneModified(true);
}

inline void InspectorTransformSetLocalPositionCommand::Undo()
{
	if (target.lock())
	{
		target.lock()->SetLocalPosition(lastValue);
		SceneManager::SetSceneModified(true);
	}
}

inline void InspectorTransformSetLocalPositionCommand::Redo()
{
	if (target.lock())
	{
		target.lock()->SetLocalPosition(newValue);
		SceneManager::SetSceneModified(true);
	}
}

//----------------------------------------------------------------------------

class InspectorTransformSetLocalRotationCommand : public Command
{
public:
	InspectorTransformSetLocalRotationCommand() = delete;
	InspectorTransformSetLocalRotationCommand(std::weak_ptr<Transform> target, Vector3 newValue, Vector3 lastValue);
	void Execute() override;
	void Undo() override;
	void Redo() override;
private:
	std::weak_ptr<Transform> target;
	Vector3 newValue;
	Vector3 lastValue;
};

inline InspectorTransformSetLocalRotationCommand::InspectorTransformSetLocalRotationCommand(std::weak_ptr<Transform> target, Vector3 newValue, Vector3 lastValue)
{
	this->target = target;
	this->newValue = newValue;
	this->lastValue = lastValue;
}

inline void InspectorTransformSetLocalRotationCommand::Execute()
{
	target.lock()->SetLocalRotation(newValue);
	SceneManager::SetSceneModified(true);
}

inline void InspectorTransformSetLocalRotationCommand::Undo()
{
	if (target.lock())
	{
		target.lock()->SetLocalRotation(lastValue);
		SceneManager::SetSceneModified(true);
	}
}

inline void InspectorTransformSetLocalRotationCommand::Redo()
{
	if (target.lock())
	{
		target.lock()->SetLocalRotation(newValue);
		SceneManager::SetSceneModified(true);
	}
}

//----------------------------------------------------------------------------

class InspectorTransformSetLocalScaleCommand : public Command
{
public:
	InspectorTransformSetLocalScaleCommand() = delete;
	InspectorTransformSetLocalScaleCommand(std::weak_ptr<Transform> target, Vector3 newValue, Vector3 lastValue);
	void Execute() override;
	void Undo() override;
	void Redo() override;
private:
	std::weak_ptr<Transform> target;
	Vector3 newValue;
	Vector3 lastValue;
};

inline InspectorTransformSetLocalScaleCommand::InspectorTransformSetLocalScaleCommand(std::weak_ptr<Transform> target, Vector3 newValue, Vector3 lastValue)
{
	this->target = target;
	this->newValue = newValue;
	this->lastValue = lastValue;
}

inline void InspectorTransformSetLocalScaleCommand::Execute()
{
	target.lock()->SetLocalScale(newValue);
	SceneManager::SetSceneModified(true);
}

inline void InspectorTransformSetLocalScaleCommand::Undo()
{
	if (target.lock())
	{
		target.lock()->SetLocalScale(lastValue);
		SceneManager::SetSceneModified(true);
	}
}

inline void InspectorTransformSetLocalScaleCommand::Redo()
{
	if (target.lock())
	{
		target.lock()->SetLocalScale(newValue);
		SceneManager::SetSceneModified(true);
	}
}

//----------------------------------------------------------------------------

template<typename T>
class InspectorAddComponentCommand : public Command
{
public:
	InspectorAddComponentCommand() = delete;
	InspectorAddComponentCommand(std::weak_ptr<GameObject> target);
	void Execute() override;
	void Undo() override;
	void Redo() override;
private:
	std::weak_ptr<GameObject> target;
	std::weak_ptr<T> newComponent;
};

template<typename T>
inline InspectorAddComponentCommand<T>::InspectorAddComponentCommand(std::weak_ptr<GameObject> target)
{
	this->target = target;
}

template<typename T>
inline void InspectorAddComponentCommand<T>::Execute()
{
	newComponent = target.lock()->AddComponent<T>();
	SceneManager::SetSceneModified(true);
}

template<typename T>
inline void InspectorAddComponentCommand<T>::Undo()
{
	if (target.lock() && newComponent.lock())
	{
		Destroy(newComponent);
		newComponent.reset();
		SceneManager::SetSceneModified(true);
	}
}

template<typename T>
inline void InspectorAddComponentCommand<T>::Redo()
{
	if (target.lock())
	{
		newComponent = target.lock()->AddComponent<T>();
		SceneManager::SetSceneModified(true);
	}
}


//----------------------------------------------------------------------------

class InspectorCreateGameObjectCommand : public Command
{
public:
	InspectorCreateGameObjectCommand() = delete;
	InspectorCreateGameObjectCommand(std::weak_ptr<GameObject> target, int mode);
	void Execute() override;
	void Undo() override;
	void Redo() override;
private:
	std::weak_ptr<GameObject> target;
	std::weak_ptr<GameObject> createdGameObject;
	std::weak_ptr<GameObject> oldParent;
	int mode; // 0 Create Empty, 1 Create Child, 2 Create parent
};

inline InspectorCreateGameObjectCommand::InspectorCreateGameObjectCommand(std::weak_ptr<GameObject> target, int mode)
{
	this->target = target;
	this->mode = mode;
}

inline void InspectorCreateGameObjectCommand::Execute()
{
	bool done = false;

	if (mode == 0)
	{
		createdGameObject = CreateGameObject();
		done = true;
	}
	else if (mode == 1 && target.lock())
	{
		createdGameObject = CreateGameObject();
		std::shared_ptr<Transform> transform = createdGameObject.lock()->GetTransform();
		createdGameObject.lock()->SetParent(target);
		transform->SetLocalPosition(Vector3(0));
		transform->SetLocalRotation(Vector3(0));
		transform->SetLocalScale(Vector3(1));
		done = true;
	}
	else if (mode == 2 && target.lock())
	{
		createdGameObject = CreateGameObject();
		std::shared_ptr<Transform> transform = createdGameObject.lock()->GetTransform();
		std::shared_ptr<Transform>  selectedTransform = target.lock()->GetTransform();
		transform->SetPosition(selectedTransform->GetPosition());
		transform->SetRotation(selectedTransform->GetRotation());
		transform->SetLocalScale(selectedTransform->GetScale());

		if (target.lock()->parent.lock())
		{
			createdGameObject.lock()->SetParent(target.lock()->parent.lock());
			oldParent = target.lock()->parent.lock();
		}
		target.lock()->SetParent(createdGameObject);
		done = true;
	}

	if (done)
	{
		Editor::SetSelectedGameObject(createdGameObject.lock());
		SceneManager::SetSceneModified(true);
	}
}

inline void InspectorCreateGameObjectCommand::Undo()
{
	if (createdGameObject.lock())
	{
		bool done = false;
		if (mode == 0)
		{
			Destroy(createdGameObject);
			done = true;
		}
		else if (mode == 1)
		{
			Destroy(createdGameObject);
			done = true;
		}
		else if (mode == 2)
		{
			if (oldParent.lock())
				target.lock()->SetParent(oldParent.lock());
			else
				target.lock()->SetParent(nullptr);
			Destroy(createdGameObject);
			done = true;
		}

		if (done)
			SceneManager::SetSceneModified(true);
	}
}

inline void InspectorCreateGameObjectCommand::Redo()
{
	Execute();
}

//----------------------------------------------------------------------------

template<typename T>
class InspectorDeleteComponentCommand : public Command
{
public:
	InspectorDeleteComponentCommand() = delete;
	InspectorDeleteComponentCommand(std::weak_ptr<T> componentToDestroy);
	void Execute() override;
	void Undo() override;
	void Redo() override;
private:
	std::weak_ptr<GameObject> target;
	std::weak_ptr<T> componentToDestroy;
	nlohmann::json componentData;
	std::string componentName = "";
};

template<typename T>
inline InspectorDeleteComponentCommand<T>::InspectorDeleteComponentCommand(std::weak_ptr<T> componentToDestroy)
{
	this->target = componentToDestroy.lock()->GetGameObject();
	this->componentToDestroy = componentToDestroy;
	this->componentData["Values"] = ReflectionUtils::MapToJson(componentToDestroy.lock()->GetReflection());
	this->componentName = componentToDestroy.lock()->GetComponentName();
}

template<typename T>
inline void InspectorDeleteComponentCommand<T>::Execute()
{
	if (componentToDestroy.lock())
	{
		Destroy(componentToDestroy);
		SceneManager::SetSceneModified(true);
	}
}

template<typename T>
inline void InspectorDeleteComponentCommand<T>::Undo()
{
	if (target.lock())
	{
		std::shared_ptr<Component> component = ClassRegistry::AddComponentFromName(componentName, target.lock());
		ReflectionUtils::JsonToMap(componentData, component->GetReflection());
		componentToDestroy = component;
		SceneManager::SetSceneModified(true);
	}
}

template<typename T>
inline void InspectorDeleteComponentCommand<T>::Redo()
{
	Execute();
}