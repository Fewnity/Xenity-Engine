// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "modify.h"

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

		SceneManager::SetSceneModified(true);
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

		SceneManager::SetSceneModified(true);
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
			foundGameObject->GetTransform()->SetLocalRotation(newValue);
		else
			foundGameObject->GetTransform()->SetRotation(newValue);
		SceneManager::SetSceneModified(true);
	}
}

void InspectorTransformSetRotationCommand::Undo()
{
	std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(targetId);
	if (foundGameObject)
	{
		if (isLocalRotation)
			foundGameObject->GetTransform()->SetLocalRotation(lastValue);
		else
			foundGameObject->GetTransform()->SetRotation(lastValue);
		SceneManager::SetSceneModified(true);
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
		SceneManager::SetSceneModified(true);
	}
}

void InspectorTransformSetLocalScaleCommand::Undo()
{
	std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(targetId);
	if (foundGameObject)
	{
		foundGameObject->GetTransform()->SetLocalScale(lastValue);
		SceneManager::SetSceneModified(true);
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
		SceneManager::SetSceneModified(true);
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
		SceneManager::SetSceneModified(true);
	}
}