// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "transform.h"

#include <glm/gtx/euler_angles.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <engine/tools/internal_math.h>
#include "gameobject.h"
#include <engine/graphics/graphics.h>
#include <engine/graphics/camera.h>

#pragma region Constructors

Transform::Transform(const std::shared_ptr<GameObject>& _gameObject) : m_gameObject(_gameObject)
{
	UpdateTransformationMatrix();
}

ReflectiveData Transform::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, m_localPosition, "localPosition");
	Reflective::AddVariable(reflectedVariables, m_localRotationQuaternion, "localRotationQuaternion");
	Reflective::AddVariable(reflectedVariables, m_localScale, "localScale");
	return reflectedVariables;
}

#pragma endregion

#pragma region Accessors

void Transform::SetTransformationMatrix(const glm::mat4& matrix)
{
	m_isTransformationMatrixDirty = false;
	transformationMatrix = matrix;

	const glm::vec3 pos = glm::vec3(matrix[3]);
	m_position = Vector3(-pos.x, pos.y, pos.z);

	const glm::quat rot = glm::quat_cast(matrix);
	m_rotationQuaternion = Quaternion(rot.x, -rot.y, -rot.z, rot.w);
	m_rotation = m_rotationQuaternion.ToEuler();
	//m_rotationQuaternion = Quaternion(rot.x, -rot.y, -rot.z, rot.w);

	transformationMatrix = glm::scale(transformationMatrix, glm::vec3(m_scale.x, m_scale.y, m_scale.z));

	const std::shared_ptr<GameObject> gm = m_gameObject.lock();
	if (gm->GetParent().expired())
	{
		// If the transform has not parent, local values are the same as world values
		m_localPosition = m_position;
		m_localRotation = m_rotationQuaternion.ToEuler();
		m_localRotationQuaternion = m_rotationQuaternion;

		SetChildrenWorldPositions();
	}
	else
	{
		SetChildrenWorldPositions();
		const std::shared_ptr<Transform> parentTransform = gm->GetParent().lock()->GetTransform();
		m_localPosition = GetLocalPositionFromMatrices(transformationMatrix, parentTransform->transformationMatrix);
		if (m_localPosition.HasInvalidValues())
			m_localPosition = Vector3(0);

		m_localRotation = GetLocalRotationFromWorldRotations(GetEulerAngles(), parentTransform->GetEulerAngles());
		m_localRotationQuaternion = Quaternion::Euler(m_localRotation.x, m_localRotation.y, m_localRotation.z);
	}
	m_onTransformUpdated.Trigger();
}

void Transform::SetPosition(const Vector3& value)
{
	XASSERT(!value.HasInvalidValues(), "[Transform::SetPosition] value is invalid");

	// Security check
	if (value.HasInvalidValues())
		return;

	if (value != m_position)
	{
		m_isTransformationMatrixDirty = true;
	}
	else
		return;

	m_position = value;

	const std::shared_ptr<GameObject> gm = m_gameObject.lock();
	if (gm->GetParent().expired())
	{
		// If the transform has not parent, local values are the same as world values
		m_localPosition = value;
		SetChildrenWorldPositions();
	}
	else
	{
		SetChildrenWorldPositions();
		m_localPosition = GetLocalPositionFromMatrices(transformationMatrix, gm->GetParent().lock()->GetTransform()->transformationMatrix);
		if (m_localPosition.HasInvalidValues())
			m_localPosition = Vector3(0);
	}
}

void Transform::SetLocalPosition(const Vector3& value)
{
	XASSERT(!value.HasInvalidValues(), "[Transform::SetLocalPosition] value is invalid");

	// Security check
	if (value.HasInvalidValues())
		return;

	if (m_gameObject.lock()->GetParent().expired())
	{
		SetPosition(value);
		return;
	}

	if (value != m_localPosition)
		m_isTransformationMatrixDirty = true;
	else
		return;

	m_localPosition = value;
	UpdateWorldValues();
}

void Transform::SetEulerAngles(const Vector3& value)
{
	XASSERT(!value.HasInvalidValues(), "[Transform::SetRotation] value is invalid");

	// Security check
	if (value.HasInvalidValues())
		return;

	// Do not update the matrix if it's the same value
	if (value != m_rotation)
		m_isTransformationMatrixDirty = true;
	else
		return;

	m_rotation = value;
	m_rotationQuaternion = Quaternion::Euler(value.x, value.y, value.z);

	const std::shared_ptr<GameObject> gm = m_gameObject.lock();
	if (gm->GetParent().expired())
	{
		// If the transform has not parent, local values are the same as world values
		m_localRotation = value;
		m_localRotationQuaternion = m_rotationQuaternion;
		SetChildrenWorldPositions();
	}
	else
	{
		SetChildrenWorldPositions();
		m_localRotation = GetLocalRotationFromWorldRotations(GetEulerAngles(), gm->GetParent().lock()->GetTransform()->GetEulerAngles());
		m_localRotationQuaternion = Quaternion::Euler(m_localRotation.x, m_localRotation.y, m_localRotation.z);
	}
}

void Transform::SetLocalEulerAngles(const Vector3& value)
{
	XASSERT(!value.HasInvalidValues(), "[Transform::SetLocalRotation] value is invalid");

	// Security check
	if (value.HasInvalidValues())
		return;

	if (m_gameObject.lock()->GetParent().expired())
	{
		SetEulerAngles(value);
		return;
	}

	if (value != m_localRotation)
		m_isTransformationMatrixDirty = true;
	else
		return;

	m_localRotation = value;
	m_localRotationQuaternion = Quaternion::Euler(value.x, value.y, value.z);
	UpdateWorldValues();
}

void Transform::SetRotation(const Quaternion& value)
{
	if (value != m_rotationQuaternion)
		m_isTransformationMatrixDirty = true;
	else
		return;

	m_rotation = value.ToEuler();
	m_rotationQuaternion = value;
	const std::shared_ptr<GameObject> gm = m_gameObject.lock();
	if (gm->GetParent().expired())
	{
		// If the transform has not parent, local values are the same as world values
		m_localRotation = value.ToEuler();
		m_localRotationQuaternion = value;
		SetChildrenWorldPositions();
	}
	else
	{
		SetChildrenWorldPositions();
		m_localRotation = GetLocalRotationFromWorldRotations(GetEulerAngles(), gm->GetParent().lock()->GetTransform()->GetEulerAngles());
		m_localRotationQuaternion = Quaternion::Euler(m_localRotation.x, m_localRotation.y, m_localRotation.z);
	}
}

void Transform::SetLocalRotation(const Quaternion& value)
{
	//XASSERT(!value.HasInvalidValues(), "[Transform::SetLocalRotation] value is invalid");

	//// Security check
	//if (value.HasInvalidValues())
	//	return;

	if (m_gameObject.lock()->GetParent().expired())
	{
		SetRotation(value);
		return;
	}

	if (value != m_localRotationQuaternion)
		m_isTransformationMatrixDirty = true;
	else
		return;

	m_localRotation = value.ToEuler();
	m_localRotationQuaternion = value;
	UpdateWorldValues();
}

void Transform::SetLocalScale(const Vector3& value)
{
	XASSERT(!value.HasInvalidValues(), "[Transform::SetLocalScale] value is invalid");

	// Security check
	if (value.HasInvalidValues())
		return;

	m_isTransformationMatrixDirty = true;

	/*if (value != localScale)
		m_isTransformationMatrixDirty = true;
	else
		return;*/

	m_localScale = value;
	UpdateWorldValues();
	m_onTransformScaled.Trigger();
}

const glm::mat4& Transform::GetMVPMatrix(size_t currentFrame)
{
	if constexpr (!s_UseOpenGLFixedFunctions)
	{
		if (currentFrame != lastMVPFrame)
		{
			lastMVPFrame = currentFrame;
			mvpMatrix = Graphics::usedCamera->m_viewProjectionMatrix * transformationMatrix;
		}
	}
	return mvpMatrix;
}

#pragma endregion

void Transform::OnParentChanged()
{
	const std::shared_ptr<GameObject> gm = m_gameObject.lock();
	if (!gm->GetParent().expired())
	{
		const std::shared_ptr<Transform>& parentTransform = gm->GetParent().lock()->GetTransform();
		//----- Set new local scale
		m_localScale = m_scale / parentTransform->m_scale;

		//----- Set new local rotation
		m_localRotation = GetLocalRotationFromWorldRotations(GetEulerAngles(), parentTransform->GetEulerAngles());
		m_localRotationQuaternion = Quaternion::Euler(m_localRotation.x, m_localRotation.y, m_localRotation.z);

		//----- Set new local position
		m_localPosition = GetLocalPositionFromMatrices(transformationMatrix, parentTransform->transformationMatrix);
	}
	else
	{
		// If the transform has not parent, local values are the same as world values
		m_localPosition = m_position;
		m_localRotation = m_rotation;
		m_localRotationQuaternion = m_rotationQuaternion;
		m_localScale = m_scale;
	}
	m_onTransformScaled.Trigger();
}

void Transform::SetChildrenWorldPositions()
{
	UpdateTransformationMatrix();

	const std::shared_ptr<GameObject> gm = m_gameObject.lock();

	const int childCount = gm->GetChildrenCount();

	//For each children
	for (int i = 0; i < childCount; i++)
	{
		const std::shared_ptr<Transform>& transform = gm->GetChildren()[i].lock()->GetTransform();
		transform->m_isTransformationMatrixDirty = true;
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
	const std::shared_ptr<GameObject> gm = m_gameObject.lock();
	if (gm->GetParent().expired())
	{
		m_rotationQuaternion = m_localRotationQuaternion;
		m_rotation = m_rotationQuaternion.ToEuler();
		m_localRotation = m_rotation;
		return;
	}

	const std::shared_ptr<Transform>& parentTransform = gm->GetParent().lock()->GetTransform();
	const Quaternion quatChildGlobal2 = parentTransform->m_rotationQuaternion * m_localRotationQuaternion;
	const Vector3 eulerChildGlobal2 = quatChildGlobal2.ToEuler();
	m_rotation = eulerChildGlobal2;
	m_rotationQuaternion = quatChildGlobal2;
}

void Transform::UpdateWorldPosition()
{
	const std::shared_ptr<GameObject> gm = m_gameObject.lock();
	if (gm->GetParent().expired())
	{
		m_position = m_localPosition;
		return;
	}

	const std::shared_ptr<Transform>& parentTransform = gm->GetParent().lock()->GetTransform();
	const Vector3& parentPosition = parentTransform->GetPosition();
	const Vector3& parentScale = parentTransform->GetScale();
	const Vector3& thisLocalPosition = GetLocalPosition();
	//Get child local position
	const float scaledLocalPos[3] = { (thisLocalPosition.x * parentScale.x), -(thisLocalPosition.y * parentScale.y), -(thisLocalPosition.z * parentScale.z) };

	//Create the matrix which store the new child's world position (wihtout parent's world position added)
	float posAfterRotation[3];
	InternalMath::MultiplyMatrices(scaledLocalPos, parentTransform->rotationMatrix, posAfterRotation, 1, 3, 3, 3);

	//Set new child position (with parent's world position added)
	m_position = Vector3(posAfterRotation[0] + parentPosition.x, (-posAfterRotation[1] + parentPosition.y), (-posAfterRotation[2] + parentPosition.z));
}

void Transform::UpdateTransformationMatrix()
{
	if (!m_isTransformationMatrixDirty)
		return;

	m_isTransformationMatrixDirty = false;
	m_isNormalMatrixDirty = true;

	transformationMatrix = glm::mat4(1.0f);

	if(m_position.x != 0.0f || m_position.y != 0.0f || m_position.z != 0.0f)
		transformationMatrix = glm::translate(transformationMatrix, glm::vec3(-m_position.x, m_position.y, m_position.z));

	const glm::mat4 RotationMatrix2 = glm::toMat4(glm::quat(m_rotationQuaternion.w, m_rotationQuaternion.x, -m_rotationQuaternion.y, -m_rotationQuaternion.z));
	transformationMatrix *= RotationMatrix2;

	for (int i = 0; i < 3; i++)
	{
		const int ix3 = i * 3;
		for (int j = 0; j < 3; j++)
		{
			rotationMatrix[ix3 + j] = transformationMatrix[i][j];
		}
	}

	transformationMatrix = glm::scale(transformationMatrix, glm::vec3(m_scale.x, m_scale.y, m_scale.z));

	m_onTransformUpdated.Trigger();
}

void Transform::UpdateWorldScale()
{
	m_scale = m_localScale;
	const std::shared_ptr<GameObject> lockGameObject = m_gameObject.lock();
	if (auto parentGm = lockGameObject->GetParent().lock())
	{
		while (parentGm != nullptr)
		{
			m_scale = m_scale * parentGm->GetTransform()->m_localScale;
			parentGm = parentGm->GetParent().lock();
		}

		const int childCount = lockGameObject->GetChildrenCount();
		for (int i = 0; i < childCount; i++)
		{
			const std::shared_ptr<GameObject> child = lockGameObject->GetChildren()[i].lock();
			child->GetTransform()->UpdateWorldScale();
		}
	}
}

void Transform::UpdateLocalRotation()
{
	m_localRotation = m_localRotationQuaternion.ToEuler();
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
