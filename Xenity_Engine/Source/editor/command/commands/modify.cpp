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
	m_targetId = _targetId;
	m_newValue = newValue;
	m_lastValue = lastValue;
	m_isLocalPosition = isLocalPosition;
}

void InspectorTransformSetPositionCommand::Execute()
{
	std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(m_targetId);
	if (foundGameObject)
	{
		if (m_isLocalPosition)
			foundGameObject->GetTransform()->SetLocalPosition(m_newValue);
		else
			foundGameObject->GetTransform()->SetPosition(m_newValue);

		SceneManager::SetIsSceneDirty(true);
	}
}

void InspectorTransformSetPositionCommand::Undo()
{
	std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(m_targetId);
	if (foundGameObject)
	{
		if (m_isLocalPosition)
			foundGameObject->GetTransform()->SetLocalPosition(m_lastValue);
		else
			foundGameObject->GetTransform()->SetPosition(m_lastValue);

		SceneManager::SetIsSceneDirty(true);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

InspectorTransformSetRotationCommand::InspectorTransformSetRotationCommand(uint64_t _targetId, const Vector3& newValue, const Vector3& lastValue, bool isLocalRotation)
{
	m_targetId = _targetId;
	m_newValue = newValue;
	m_lastValue = lastValue;
	m_isLocalRotation = isLocalRotation;
}

void InspectorTransformSetRotationCommand::Execute()
{
	std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(m_targetId);
	if (foundGameObject)
	{
		if (m_isLocalRotation)
			foundGameObject->GetTransform()->SetLocalEulerAngles(m_newValue);
		else
			foundGameObject->GetTransform()->SetEulerAngles(m_newValue);
		SceneManager::SetIsSceneDirty(true);
	}
}

void InspectorTransformSetRotationCommand::Undo()
{
	std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(m_targetId);
	if (foundGameObject)
	{
		if (m_isLocalRotation)
			foundGameObject->GetTransform()->SetLocalEulerAngles(m_lastValue);
		else
			foundGameObject->GetTransform()->SetEulerAngles(m_lastValue);
		SceneManager::SetIsSceneDirty(true);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

InspectorTransformSetLocalScaleCommand::InspectorTransformSetLocalScaleCommand(uint64_t _targetId, const Vector3& _newValue, const Vector3& _lastValue) : m_newValue(_newValue), m_lastValue(_lastValue)
{
	m_targetId = _targetId;
}

void InspectorTransformSetLocalScaleCommand::Execute()
{
	std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(m_targetId);
	if (foundGameObject)
	{
		foundGameObject->GetTransform()->SetLocalScale(m_newValue);
		SceneManager::SetIsSceneDirty(true);
	}
}

void InspectorTransformSetLocalScaleCommand::Undo()
{
	std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(m_targetId);
	if (foundGameObject)
	{
		foundGameObject->GetTransform()->SetLocalScale(m_lastValue);
		SceneManager::SetIsSceneDirty(true);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

InspectorSetTransformDataCommand::InspectorSetTransformDataCommand(Transform& transform, const nlohmann::json& newTransformDataData) : m_transformData(newTransformDataData)
{
	m_transformtId = transform.GetGameObject()->GetUniqueId();
	m_oldTransformData["Values"] = ReflectionUtils::ReflectiveDataToJson(transform.GetReflectiveData());
}

void InspectorSetTransformDataCommand::Execute()
{
	std::shared_ptr<GameObject> gameObject = FindGameObjectById(m_transformtId);
	if (gameObject)
	{
		std::shared_ptr<Transform> transformToUpdate = gameObject->GetTransform();
		ReflectionUtils::JsonToReflectiveData(m_transformData, transformToUpdate->GetReflectiveData());
		transformToUpdate->m_isTransformationMatrixDirty = true;
		transformToUpdate->UpdateWorldValues();
		transformToUpdate->OnReflectionUpdated();
		SceneManager::SetIsSceneDirty(true);
	}
}

void InspectorSetTransformDataCommand::Undo()
{
	std::shared_ptr<GameObject> gameObject = FindGameObjectById(m_transformtId);
	if (gameObject)
	{
		std::shared_ptr<Transform> transformToUpdate = gameObject->GetTransform();
		ReflectionUtils::JsonToReflectiveData(m_oldTransformData, transformToUpdate->GetReflectiveData());
		transformToUpdate->m_isTransformationMatrixDirty = true;
		transformToUpdate->UpdateWorldValues();
		transformToUpdate->OnReflectionUpdated();
		SceneManager::SetIsSceneDirty(true);
	}
}
