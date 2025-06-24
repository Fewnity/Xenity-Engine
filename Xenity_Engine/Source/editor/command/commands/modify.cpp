// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "modify.h"
#include <engine/game_elements/rect_transform.h>

InspectorRectTransformSetPositionCommand::InspectorRectTransformSetPositionCommand(uint64_t _targetId, const Vector2& newValue, const Vector2& lastValue)
{
	m_targetId = _targetId;
	m_newValue = newValue;
	m_lastValue = lastValue;
}

void InspectorRectTransformSetPositionCommand::Execute()
{
	std::shared_ptr<Component> foundGameObject = FindComponentById(m_targetId);
	if (foundGameObject)
	{
		std::dynamic_pointer_cast<RectTransform>(foundGameObject)->position = m_newValue;
		SceneManager::SetIsSceneDirty(true);
	}
}

void InspectorRectTransformSetPositionCommand::Undo()
{
	std::shared_ptr<Component> foundGameObject = FindComponentById(m_targetId);
	if (foundGameObject)
	{
		std::dynamic_pointer_cast<RectTransform>(foundGameObject)->position = m_lastValue;
		SceneManager::SetIsSceneDirty(true);
	}
}

InspectorTransformSetPositionCommand::InspectorTransformSetPositionCommand(uint64_t _targetId, const Vector3& newValue, const Vector3& lastValue, bool isLocalPosition)
{
	this->targetId = _targetId;
	this->newValue = newValue;
	this->lastValue = lastValue;
	this->isLocalPosition = isLocalPosition;
}

void InspectorTransformSetPositionCommand::Execute()
{
	std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(targetId);
	if (foundGameObject)
	{
		if (isLocalPosition)
			foundGameObject->GetTransform()->SetLocalPosition(newValue);
		else
			foundGameObject->GetTransform()->SetPosition(newValue);

		SceneManager::SetIsSceneDirty(true);
	}
}

void InspectorTransformSetPositionCommand::Undo()
{
	std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(targetId);
	if (foundGameObject)
	{
		if (isLocalPosition)
			foundGameObject->GetTransform()->SetLocalPosition(lastValue);
		else
			foundGameObject->GetTransform()->SetPosition(lastValue);

		SceneManager::SetIsSceneDirty(true);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

InspectorTransformSetRotationCommand::InspectorTransformSetRotationCommand(uint64_t _targetId, const Vector3& newValue, const Vector3& lastValue, bool isLocalRotation)
{
	this->targetId = _targetId;
	this->newValue = newValue;
	this->lastValue = lastValue;
	this->isLocalRotation = isLocalRotation;
}

void InspectorTransformSetRotationCommand::Execute()
{
	std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(targetId);
	if (foundGameObject)
	{
		if (isLocalRotation)
			foundGameObject->GetTransform()->SetLocalEulerAngles(newValue);
		else
			foundGameObject->GetTransform()->SetEulerAngles(newValue);
		SceneManager::SetIsSceneDirty(true);
	}
}

void InspectorTransformSetRotationCommand::Undo()
{
	std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(targetId);
	if (foundGameObject)
	{
		if (isLocalRotation)
			foundGameObject->GetTransform()->SetLocalEulerAngles(lastValue);
		else
			foundGameObject->GetTransform()->SetEulerAngles(lastValue);
		SceneManager::SetIsSceneDirty(true);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

InspectorTransformSetLocalScaleCommand::InspectorTransformSetLocalScaleCommand(uint64_t _targetId, const Vector3& _newValue, const Vector3& _lastValue) : newValue(_newValue), lastValue(_lastValue)
{
	this->targetId = _targetId;
}

void InspectorTransformSetLocalScaleCommand::Execute()
{
	std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(targetId);
	if (foundGameObject)
	{
		foundGameObject->GetTransform()->SetLocalScale(newValue);
		SceneManager::SetIsSceneDirty(true);
	}
}

void InspectorTransformSetLocalScaleCommand::Undo()
{
	std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(targetId);
	if (foundGameObject)
	{
		foundGameObject->GetTransform()->SetLocalScale(lastValue);
		SceneManager::SetIsSceneDirty(true);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

InspectorSetTransformDataCommand::InspectorSetTransformDataCommand(Transform& transform, const nlohmann::json& newTransformDataData) : transformData(newTransformDataData)
{
	this->transformtId = transform.GetGameObject()->GetUniqueId();
	this->oldTransformData["Values"] = ReflectionUtils::ReflectiveDataToJson(transform.GetReflectiveData());
}

void InspectorSetTransformDataCommand::Execute()
{
	std::shared_ptr<GameObject> gameObject = FindGameObjectById(transformtId);
	if (gameObject)
	{
		std::shared_ptr<Transform> transformToUpdate = gameObject->GetTransform();
		ReflectionUtils::JsonToReflectiveData(transformData, transformToUpdate->GetReflectiveData());
		transformToUpdate->m_isTransformationMatrixDirty = true;
		transformToUpdate->UpdateWorldValues();
		transformToUpdate->OnReflectionUpdated();
		SceneManager::SetIsSceneDirty(true);
	}
}

void InspectorSetTransformDataCommand::Undo()
{
	std::shared_ptr<GameObject> gameObject = FindGameObjectById(transformtId);
	if (gameObject)
	{
		std::shared_ptr<Transform> transformToUpdate = gameObject->GetTransform();
		ReflectionUtils::JsonToReflectiveData(oldTransformData, transformToUpdate->GetReflectiveData());
		transformToUpdate->m_isTransformationMatrixDirty = true;
		transformToUpdate->UpdateWorldValues();
		transformToUpdate->OnReflectionUpdated();
		SceneManager::SetIsSceneDirty(true);
	}
}