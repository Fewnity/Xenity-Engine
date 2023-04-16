#include "transform.h"
#include "gameobject.h"
#include <iostream>
#include "tools/math.h"
#include <glm/ext/matrix_transform.hpp>
#include <SDL2/SDL_stdinc.h>
#include "tools/math.h"

#pragma region Constructors

Transform::Transform(GameObject* gameObject)
{
	this->gameObject = gameObject;
	UpdateTransformationMatrix();
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
	Vector3 direction = Vector3(rotationMatrix[6], rotationMatrix[7], rotationMatrix[8]);
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
	Vector3 direction = Vector3(rotationMatrix[0], rotationMatrix[1], rotationMatrix[2]);
	return direction;
}

Vector3 Transform::GetUp() const
{
	Vector3 direction = Vector3(rotationMatrix[3], rotationMatrix[4], rotationMatrix[5]);
	return direction;
}

Vector3 Transform::GetDown() const
{
	return -GetUp();
}

void Transform::SetPosition(const Vector3 value)
{
	if (value != position)
		isTransformationMatrixDirty = true;
	else
		return;

	position = value;
	if (gameObject->parent == nullptr)
	{
		localPosition = value;
	}
	else
	{
		localPosition = (position - gameObject->parent->transform.position) * localScale;
	}

	SetChildrenWorldPositions();
}

void Transform::SetLocalPosition(const Vector3 value)
{
	if (gameObject->parent == nullptr) 
	{
		SetPosition(value);
		return;
	}

	if (value != localPosition)
		isTransformationMatrixDirty = true;
	else
		return;

	localPosition = value;
	UpdateWorldValues();
}

void Transform::SetRotation(const Vector3 value)
{
	if (value != rotation)
		isTransformationMatrixDirty = true;
	else
		return;

	rotation = value;
	if (gameObject->parent == nullptr)
	{
		localRotation = value;
	}
	else
	{
		localRotation = rotation - gameObject->parent->transform.rotation;
	}

	SetChildrenWorldPositions();
}

void Transform::SetLocalRotation(const Vector3 value)
{
	if (gameObject->parent == nullptr)
	{
		SetRotation(value);
		return;
	}

	if (value != localRotation)
		isTransformationMatrixDirty = true;
	else
		return;

	localRotation = value;
	UpdateWorldValues();
}

void Transform::SetLocalScale(const Vector3 value)
{
	if (value != localScale)
		isTransformationMatrixDirty = true;
	else
		return;

	localScale = value;
	UpdateWorldValues();
}

#pragma endregion

void Transform::OnParentChanged()
{
	if (gameObject->parent != nullptr)
	{
		//Create the matrix which store the new child's world position (wihtout parent's world position added)
		float modifiedMatrix[9];

		for (int i = 0; i < 9; i++)
		{
			modifiedMatrix[i] = gameObject->parent->transform.rotationMatrix[i];
		}
		//invert angle
		modifiedMatrix[1] = -modifiedMatrix[1];
		modifiedMatrix[2] = -modifiedMatrix[2];
		modifiedMatrix[3] = -modifiedMatrix[3];

		float posAfterRotation[3];
		localScale = scale / gameObject->parent->transform.scale;
		localRotation = rotation - gameObject->parent->transform.rotation;

		Vector3 localPosition2 = (position - gameObject->parent->transform.position) / gameObject->parent->transform.scale;
		float localPos[3] = { localPosition2.x, localPosition2.y , localPosition2.z };
		Math::MultiplyMatrix(localPos, modifiedMatrix, posAfterRotation, 1, 3, 3, 3);
		localPosition = Vector3(posAfterRotation[0], posAfterRotation[1], posAfterRotation[2]);
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
	//std::cout << gameObject->name << std::endl;
	/*std::cout << gameObject->name << std::endl;
	std::cout << "TRANSFORM:" << std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			std::cout << transformationMatrix[i][j] << " ";
		}
		std::cout << "|" << std::endl;
	}
	std::cout << "ROTATION:" << std::endl;
	for (int i = 0; i < 9; i++)
	{
		std::cout << rotationMatrix[i] << std::endl;
	}
	std::cout << "|" << std::endl;*/

	int childCount = gameObject->GetChildrenCount();

	//For each children
	for (int i = 0; i < childCount; i++)
	{
		gameObject->children[i]->transform.isTransformationMatrixDirty = true;
		gameObject->children[i]->transform.UpdateWorldValues();
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
	if (gameObject->parent == nullptr)
		return;

	Vector3 newRotation;
	newRotation = gameObject->parent->transform.GetRotation() + GetLocalRotation();

	rotation = newRotation;
}

void Transform::UpdateWorldPosition()
{
	if (gameObject->parent == nullptr)
		return;

	//Get child local position
	float scaledLocalPos[3] = { GetLocalPosition().x * gameObject->parent->transform.GetScale().x, GetLocalPosition().y * gameObject->parent->transform.GetScale().y, GetLocalPosition().z * gameObject->parent->transform.GetScale().z };

	//Create the matrix which store the new child's world position (wihtout parent's world position added)
	float posAfterRotation[3];
	Math::MultiplyMatrix(scaledLocalPos, gameObject->parent->transform.rotationMatrix, posAfterRotation, 1, 3, 3, 3);

	//Set new child position (with parent's world position added)
	position = Vector3(-posAfterRotation[0] + gameObject->parent->transform.GetPosition().x, posAfterRotation[1] + gameObject->parent->transform.GetPosition().y, posAfterRotation[2] + gameObject->parent->transform.GetPosition().z);
}

void Transform::UpdateTransformationMatrix()
{
	if (!isTransformationMatrixDirty)
		return;

	isTransformationMatrixDirty = false;

	transformationMatrix = glm::mat4(1.0f);

	transformationMatrix = glm::translate(transformationMatrix, glm::vec3(-position.x, position.y, position.z));
	transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

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

	GameObject* parentGm = gameObject->parent;
	while (parentGm != nullptr)
	{
		scale = scale * parentGm->transform.localScale;
		parentGm = parentGm->parent;
	}

	int childCount = gameObject->GetChildrenCount();
	for (int i = 0; i < childCount; i++)
	{
		GameObject* child = gameObject->children[i];
		child->transform.UpdateWorldScale();
	}
}