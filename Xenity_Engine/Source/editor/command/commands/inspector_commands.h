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