#include "transform.h"
#include "gameobject.h"
#include "../engine.h"
#include <iostream>
#include "../tools/math.h"
#include <glm/gtx/euler_angles.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../inputs/input_system.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#pragma region Constructors

Transform::Transform(const std::weak_ptr<GameObject>& gameObject)
{
	this->gameObject = gameObject;
	UpdateTransformationMatrix();
}

std::unordered_map<std::string, ReflectionEntry> Transform::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	Reflection::AddReflectionVariable(reflectedVariables, localPosition, "localPosition", true);
	Reflection::AddReflectionVariable(reflectedVariables, localRotation, "localRotation", true);
	Reflection::AddReflectionVariable(reflectedVariables, localScale, "localScale", true);
	return reflectedVariables;
}

#pragma endregion

#pragma region Accessors

Vector3 Transform::GetPosition() const
{
	return position;
}

Vector3 Transform::GetLocalPosition() const
{
	return localPosition;
}

Vector3 Transform::GetRotation() const
{
	return rotation;
}

Vector3 Transform::GetLocalRotation() const
{
	return localRotation;
}

Vector3 Transform::GetScale() const
{
	return scale;
}

Vector3 Transform::GetLocalScale() const
{
	return localScale;
}

Vector3 Transform::GetForward() const
{
	Vector3 direction = Vector3(-rotationMatrix[6], rotationMatrix[7], rotationMatrix[8]);
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
	Vector3 direction = Vector3(rotationMatrix[0], -rotationMatrix[1], -rotationMatrix[2]);
	return direction;
}

Vector3 Transform::GetUp() const
{
	Vector3 direction = Vector3(-rotationMatrix[3], rotationMatrix[4], rotationMatrix[5]);
	return direction;
}

Vector3 Transform::GetDown() const
{
	return -GetUp();
}

void Transform::SetPosition(const Vector3& value)
{
	if (value != position)
	{
		isTransformationMatrixDirty = true;
	}
	else
		return;

	position = value;

	auto gm = gameObject.lock();
	if (gm->parent.expired())
	{
		localPosition = value;
	}
	else
	{
		localPosition = GetLocalPositionFromMatrices(transformationMatrix, gm->parent.lock()->GetTransform()->transformationMatrix);
	}

	SetChildrenWorldPositions();
}

void Transform::SetLocalPosition(const Vector3& value)
{
	if (gameObject.lock()->parent.expired())
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
	if (value != rotation)
		isTransformationMatrixDirty = true;
	else
		return;

	rotation = value;
	auto gm = gameObject.lock();
	if (gm->parent.expired())
	{
		localRotation = value;
	}
	else
	{
		localRotation = GetLocalRotationFromWorldRotations(GetRotation(), gm->parent.lock()->GetTransform()->GetRotation());
	}

	SetChildrenWorldPositions();
}

void Transform::SetLocalRotation(const Vector3& value)
{
	if (gameObject.lock()->parent.expired())
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

void Transform::SetLocalScale(const Vector3& value)
{
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
	auto gm = gameObject.lock();
	if (!gm->parent.expired())
	{
		auto parentTransform = gm->parent.lock()->GetTransform();
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
	movedLastFrame = true;
	UpdateTransformationMatrix();

	auto gm = gameObject.lock();

	int childCount = gm->GetChildrenCount();

	//For each children
	for (int i = 0; i < childCount; i++)
	{
		auto transform = gm->children[i].lock()->GetTransform();
		transform->isTransformationMatrixDirty = true;
		transform->UpdateWorldValues();
	}
}

void Transform::UpdateWorldValues()
{
	movedLastFrame = true;
	UpdateWorldPosition();
	UpdateWorldRotation();
	UpdateWorldScale();

	SetChildrenWorldPositions();
}

void Transform::UpdateWorldRotation()
{
	auto gm = gameObject.lock();
	if (gm->parent.expired())
	{
		rotation = localRotation;
		return;
	}

	auto parentTransform = gm->parent.lock()->GetTransform();
	Vector3 parentRotation = parentTransform->GetRotation();
	glm::quat quatParentGlobal = glm::quat(glm::radians(glm::vec3(parentRotation.z, parentRotation.x, parentRotation.y)));
	glm::quat quatChildLocal = glm::quat(glm::radians(glm::vec3(localRotation.z, localRotation.x, localRotation.y)));

	glm::quat quatChildGlobal = quatParentGlobal * quatChildLocal;

	glm::vec3 eulerChildGlobal = glm::degrees(glm::eulerAngles(quatChildGlobal));

	Vector3 newRotation;
	newRotation.x = eulerChildGlobal.y;
	newRotation.y = eulerChildGlobal.z;
	newRotation.z = eulerChildGlobal.x;

	rotation = newRotation;
}

void Transform::UpdateWorldPosition()
{
	auto gm = gameObject.lock();
	if (gm->parent.expired())
	{
		position = localPosition;
		return;
	}

	auto parentTransform = gm->parent.lock()->GetTransform();
	Vector3 parentPosition = parentTransform->GetPosition();
	Vector3 parentScale = parentTransform->GetScale();
	Vector3 thisLocalPosition = GetLocalPosition();
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

	/*if (drawableCount != 0)
	{
		Engine::drawOrderListDirty = true;
	}*/

	isTransformationMatrixDirty = false;

	transformationMatrix = glm::mat4(1.0f);

	transformationMatrix = glm::translate(transformationMatrix, glm::vec3(-position.x, position.y, position.z));

	transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.y * -1), glm::vec3(0.0f, 1.0f, 0.0f));
	transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.z * -1), glm::vec3(0.0f, 0.0f, 1.0f));

	for (int i = 0; i < 3; i++)
	{
		int ix3 = i * 3;
		for (int j = 0; j < 3; j++)
		{
			rotationMatrix[ix3 + j] = transformationMatrix[i][j];
		}
	}

	transformationMatrix = glm::scale(transformationMatrix, glm::vec3(scale.x, scale.y, scale.z));
}

void Transform::UpdateWorldScale()
{
	scale = localScale;
	auto lockGameObject = gameObject.lock();
	if (auto parentGm = lockGameObject->parent.lock())
	{
		while (parentGm != nullptr)
		{
			scale = scale * parentGm->GetTransform()->localScale;
			parentGm = parentGm->parent.lock();
		}

		int childCount = lockGameObject->GetChildrenCount();
		for (int i = 0; i < childCount; i++)
		{
			auto child = lockGameObject->children[i].lock();
			child->GetTransform()->UpdateWorldScale();
		}
	}
}

Vector3 Transform::GetLocalPositionFromMatrices(const glm::mat4& childMatrix, const glm::mat4& parentMatrix) const
{
	glm::mat4 parentGlobalTransformInverse = glm::inverse(parentMatrix);
	glm::mat4 childLocalTransform = parentGlobalTransformInverse * childMatrix;

	glm::vec3 childLocalPosition = glm::vec3(childLocalTransform[3]);

	return Vector3(-childLocalPosition.x, childLocalPosition.y, childLocalPosition.z);
}

Vector3 Transform::GetLocalRotationFromWorldRotations(const Vector3& childWorldRotation, const Vector3& parentWorldRotation) const
{
	glm::quat quatParentGlobal = glm::quat(glm::radians(glm::vec3(parentWorldRotation.z, parentWorldRotation.x, parentWorldRotation.y)));
	glm::quat quatChildGlobal = glm::quat(glm::radians(glm::vec3(childWorldRotation.z, childWorldRotation.x, childWorldRotation.y)));

	glm::quat quatChildGlobalRelativeToParentInverse = glm::inverse(quatParentGlobal) * quatChildGlobal;

	float tempx = -quatChildGlobalRelativeToParentInverse.x;
	float tempy = -quatChildGlobalRelativeToParentInverse.y;
	float tempz = -quatChildGlobalRelativeToParentInverse.z;
	float tempw = -quatChildGlobalRelativeToParentInverse.w;

	quatChildGlobalRelativeToParentInverse.x = tempy;
	quatChildGlobalRelativeToParentInverse.y = tempz;
	quatChildGlobalRelativeToParentInverse.z = tempx;
	quatChildGlobalRelativeToParentInverse.w = tempw;

	glm::mat4 matChildRelative = glm::mat4_cast(quatChildGlobalRelativeToParentInverse);

	float x, y, z;
	glm::extractEulerAngleYXZ(matChildRelative, x, y, z);
	x = glm::degrees(x);
	y = glm::degrees(y);
	z = glm::degrees(z);

	return Vector3(y, x, z);
}
