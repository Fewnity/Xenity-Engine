// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "transform.h"
#include "gameobject.h"

#include <engine/tools/math.h>
#include <glm/gtx/euler_angles.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#pragma region Constructors

Transform::Transform(const std::shared_ptr<GameObject>& _gameObject) : gameObject(_gameObject)
{
	UpdateTransformationMatrix();
}

ReflectiveData Transform::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, localPosition, "localPosition", true);
	Reflective::AddVariable(reflectedVariables, localRotation, "localRotation", true);
	Reflective::AddVariable(reflectedVariables, localScale, "localScale", true);
	return reflectedVariables;
}

#pragma endregion

#pragma region Accessors

Vector3 Transform::GetForward() const
{
	const Vector3 direction = Vector3(-rotationMatrix[6], rotationMatrix[7], rotationMatrix[8]);
	return direction;
}

Vector3 Transform::GetBackward() const
{
	return -GetForward();
}

Vector3 Transform::GetLeft() const
{
	return -GetRight();
}

Vector3 Transform::GetRight() const
{
	const Vector3 direction = Vector3(rotationMatrix[0], -rotationMatrix[1], -rotationMatrix[2]);
	return direction;
}

Vector3 Transform::GetUp() const
{
	const Vector3 direction = Vector3(-rotationMatrix[3], rotationMatrix[4], rotationMatrix[5]);
	return direction;
}

Vector3 Transform::GetDown() const
{
	return -GetUp();
}

void Transform::SetPosition(const Vector3& value)
{
	XASSERT(!value.HasInvalidValues(), "[Transform::SetPosition] value is invalid");

	// Security check
	if (value.HasInvalidValues())
		return;

	if (value != position)
	{
		isTransformationMatrixDirty = true;
	}
	else
		return;

	position = value;

	const std::shared_ptr<GameObject> gm = gameObject.lock();
	if (gm->GetParent().expired())
	{
		localPosition = value;
		SetChildrenWorldPositions();
	}
	else
	{
		SetChildrenWorldPositions();
		localPosition = GetLocalPositionFromMatrices(transformationMatrix, gm->GetParent().lock()->GetTransform()->transformationMatrix);
	}
	if (localPosition.HasInvalidValues())
		localPosition = Vector3(0);
}

void Transform::SetLocalPosition(const Vector3& value)
{
	XASSERT(!value.HasInvalidValues(), "[Transform::SetLocalPosition] value is invalid");

	// Security check
	if (value.HasInvalidValues())
		return;

	if (gameObject.lock()->GetParent().expired())
	{
		SetPosition(value);
		return;
	}

	isTransformationMatrixDirty = true;

	/*if (value != localPosition)
		isTransformationMatrixDirty = true;
	else
		return;*/

	localPosition = value;
	UpdateWorldValues();
}

void Transform::SetRotation(const Vector3& value)
{
	XASSERT(!value.HasInvalidValues(), "[Transform::SetRotation] value is invalid");

	// Security check
	if (value.HasInvalidValues())
		return;

	// Do not update the matrix if it's the same value
	if (value != rotation)
		isTransformationMatrixDirty = true;
	else
		return;

	rotation = value;
	const std::shared_ptr<GameObject> gm = gameObject.lock();
	if (gm->GetParent().expired())
	{
		localRotation = value;
		SetChildrenWorldPositions();
	}
	else
	{
		SetChildrenWorldPositions();
		localRotation = GetLocalRotationFromWorldRotations(GetRotation(), gm->GetParent().lock()->GetTransform()->GetRotation());
	}
}

void Transform::SetLocalRotation(const Vector3& value)
{
	XASSERT(!value.HasInvalidValues(), "[Transform::SetLocalRotation] value is invalid");

	// Security check
	if (value.HasInvalidValues())
		return;

	if (gameObject.lock()->GetParent().expired())
	{
		SetRotation(value);
		return;
	}

	/*if (value != localRotation)
		isTransformationMatrixDirty = true;
	else
		return;*/

	isTransformationMatrixDirty = true;

	localRotation = value;
	UpdateWorldValues();
}

void Transform::SetRotation(const Quaternion& value)
{
	isTransformationMatrixDirty = true;

	rotationQuaternion = value;
	const std::shared_ptr<GameObject> gm = gameObject.lock();
	if (gm->GetParent().expired())
	{
		localRotationQuaternion = value;
		SetChildrenWorldPositions();
	}
	else
	{
		SetChildrenWorldPositions();
		//localRotation = GetLocalRotationFromWorldRotations(GetRotation(), gm->GetParent().lock()->GetTransform()->GetRotation());
	}
}

void Transform::SetLocalRotation(const Quaternion& value)
{
	//XASSERT(!value.HasInvalidValues(), "[Transform::SetLocalRotation] value is invalid");

	//// Security check
	//if (value.HasInvalidValues())
	//	return;

	if (gameObject.lock()->GetParent().expired())
	{
		SetRotation(value);
		return;
	}

	/*if (value != localRotation)
		isTransformationMatrixDirty = true;
	else
		return;*/

	isTransformationMatrixDirty = true;

	localRotationQuaternion = value;
	UpdateWorldValues();
}

void Transform::SetLocalScale(const Vector3& value)
{
	XASSERT(!value.HasInvalidValues(), "[Transform::SetLocalScale] value is invalid");

	// Security check
	if (value.HasInvalidValues())
		return;

	isTransformationMatrixDirty = true;

	/*if (value != localScale)
		isTransformationMatrixDirty = true;
	else
		return;*/

	localScale = value;
	UpdateWorldValues();
}

#pragma endregion

void Transform::OnParentChanged()
{
	const std::shared_ptr<GameObject> gm = gameObject.lock();
	if (!gm->GetParent().expired())
	{
		std::shared_ptr<Transform> parentTransform = gm->GetParent().lock()->GetTransform();
		//----- Set new local scale
		localScale = scale / parentTransform->scale;

		//----- Set new local rotation
		localRotation = GetLocalRotationFromWorldRotations(GetRotation(), parentTransform->GetRotation());

		//----- Set new local position
		localPosition = GetLocalPositionFromMatrices(transformationMatrix, parentTransform->transformationMatrix);
	}
	else
	{
		localPosition = position;
		localRotation = rotation;
		localScale = scale;
	}
}

void Transform::SetChildrenWorldPositions()
{
	UpdateTransformationMatrix();

	const std::shared_ptr<GameObject> gm = gameObject.lock();

	const int childCount = gm->GetChildrenCount();

	//For each children
	for (int i = 0; i < childCount; i++)
	{
		std::shared_ptr<Transform> transform = gm->GetChildren()[i].lock()->GetTransform();
		transform->isTransformationMatrixDirty = true;
		transform->UpdateWorldValues();
	}
}

void Transform::UpdateWorldValues()
{
	UpdateWorldPosition();
	UpdateWorldRotation();
	UpdateWorldScale();

	SetChildrenWorldPositions();
}

void Transform::UpdateWorldRotation()
{
	const std::shared_ptr<GameObject> gm = gameObject.lock();
	if (gm->GetParent().expired())
	{
		rotation = localRotation;
		rotationQuaternion = localRotationQuaternion;
		return;
	}

	const std::shared_ptr<Transform> parentTransform = gm->GetParent().lock()->GetTransform();
	const Vector3& parentRotation = parentTransform->GetRotation();
	const glm::quat quatParentGlobal = glm::quat(glm::radians(glm::vec3(parentRotation.z, parentRotation.x, parentRotation.y)));
	const glm::quat quatChildLocal = glm::quat(glm::radians(glm::vec3(localRotation.z, localRotation.x, localRotation.y)));

	const glm::quat quatChildGlobal = quatParentGlobal * quatChildLocal;

	const glm::vec3 eulerChildGlobal = glm::degrees(glm::eulerAngles(quatChildGlobal));

	Vector3 newRotation;
	newRotation.x = eulerChildGlobal.y;
	newRotation.y = eulerChildGlobal.z;
	newRotation.z = eulerChildGlobal.x;

	rotation = newRotation;
}

void Transform::UpdateWorldPosition()
{
	const std::shared_ptr<GameObject> gm = gameObject.lock();
	if (gm->GetParent().expired())
	{
		position = localPosition;
		return;
	}

	auto parentTransform = gm->GetParent().lock()->GetTransform();
	const Vector3& parentPosition = parentTransform->GetPosition();
	const Vector3& parentScale = parentTransform->GetScale();
	const Vector3& thisLocalPosition = GetLocalPosition();
	//Get child local position
	const float scaledLocalPos[3] = { (thisLocalPosition.x * parentScale.x), -(thisLocalPosition.y * parentScale.y), -(thisLocalPosition.z * parentScale.z) };

	//Create the matrix which store the new child's world position (wihtout parent's world position added)
	float posAfterRotation[3];
	Math::MultiplyMatrices(scaledLocalPos, parentTransform->rotationMatrix, posAfterRotation, 1, 3, 3, 3);

	//Set new child position (with parent's world position added)
	position = Vector3(posAfterRotation[0] + parentPosition.x, (-posAfterRotation[1] + parentPosition.y), (-posAfterRotation[2] + parentPosition.z));
}

void Transform::UpdateTransformationMatrix()
{
	if (!isTransformationMatrixDirty)
		return;

	isTransformationMatrixDirty = false;

	transformationMatrix = glm::mat4(1.0f);

	if(position.x != 0.0f || position.y != 0.0f || position.z != 0.0f)
		transformationMatrix = glm::translate(transformationMatrix, glm::vec3(-position.x, position.y, position.z));

	/*if(rotation.y != 0.0f)
		transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.y * -1), glm::vec3(0.0f, 1.0f, 0.0f));
	if (rotation.x != 0.0f)
		transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	if (rotation.z != 0.0f)
		transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.z * -1), glm::vec3(0.0f, 0.0f, 1.0f));*/

	//transformationMatrix = glm::rotate(transformationMatrix, glm::quat(2));
	glm::mat4 RotationMatrix2 = glm::toMat4(glm::quat(-rotationQuaternion.w, rotationQuaternion.z, rotationQuaternion.y, -rotationQuaternion.x));
	transformationMatrix *= RotationMatrix2;
	//transformationMatrix *= glm::quat(rotationQuaternion.w, rotationQuaternion.x, rotationQuaternion.y, rotationQuaternion.z);

	for (int i = 0; i < 3; i++)
	{
		const int ix3 = i * 3;
		for (int j = 0; j < 3; j++)
		{
			rotationMatrix[ix3 + j] = transformationMatrix[i][j];
		}
	}

	transformationMatrix = glm::scale(transformationMatrix, glm::vec3(scale.x, scale.y, scale.z));

	onTransformUpdated.Trigger();
}

void Transform::UpdateWorldScale()
{
	scale = localScale;
	const std::shared_ptr<GameObject> lockGameObject = gameObject.lock();
	if (auto parentGm = lockGameObject->GetParent().lock())
	{
		while (parentGm != nullptr)
		{
			scale = scale * parentGm->GetTransform()->localScale;
			parentGm = parentGm->GetParent().lock();
		}

		const int childCount = lockGameObject->GetChildrenCount();
		for (int i = 0; i < childCount; i++)
		{
			std::shared_ptr<GameObject> child = lockGameObject->GetChildren()[i].lock();
			child->GetTransform()->UpdateWorldScale();
		}
	}
}

Vector3 Transform::GetLocalPositionFromMatrices(const glm::mat4& childMatrix, const glm::mat4& parentMatrix) const
{
	const glm::mat4 parentGlobalTransformInverse = glm::inverse(parentMatrix);
	const glm::mat4 childLocalTransform = parentGlobalTransformInverse * childMatrix;

	const glm::vec3 childLocalPosition = glm::vec3(childLocalTransform[3]);

	return Vector3(-childLocalPosition.x, childLocalPosition.y, childLocalPosition.z);
}

Vector3 Transform::GetLocalRotationFromWorldRotations(const Vector3& childWorldRotation, const Vector3& parentWorldRotation) const
{
	const glm::quat quatParentGlobal = glm::quat(glm::radians(glm::vec3(parentWorldRotation.z, parentWorldRotation.x, parentWorldRotation.y)));
	const glm::quat quatChildGlobal = glm::quat(glm::radians(glm::vec3(childWorldRotation.z, childWorldRotation.x, childWorldRotation.y)));

	glm::quat quatChildGlobalRelativeToParentInverse = glm::inverse(quatParentGlobal) * quatChildGlobal;

	const float tempx = -quatChildGlobalRelativeToParentInverse.x;
	const float tempy = -quatChildGlobalRelativeToParentInverse.y;
	const float tempz = -quatChildGlobalRelativeToParentInverse.z;
	const float tempw = -quatChildGlobalRelativeToParentInverse.w;

	quatChildGlobalRelativeToParentInverse.x = tempy;
	quatChildGlobalRelativeToParentInverse.y = tempz;
	quatChildGlobalRelativeToParentInverse.z = tempx;
	quatChildGlobalRelativeToParentInverse.w = tempw;

	const glm::mat4 matChildRelative = glm::mat4_cast(quatChildGlobalRelativeToParentInverse);

	float x, y, z;
	glm::extractEulerAngleYXZ(matChildRelative, x, y, z);
	x = glm::degrees(x);
	y = glm::degrees(y);
	z = glm::degrees(z);

	return Vector3(y, x, z); // and not x, y, z
}
