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
#include <SDL2/SDL_stdinc.h>
#include "../inputs/input_system.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#pragma region Constructors

Transform::Transform(std::weak_ptr<GameObject> gameObject)
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

void Transform::SetPosition(const Vector3 value)
{
	if (value != position)
	{
		isTransformationMatrixDirty = true;
	}
	else
		return;

	position = value;
	if (gameObject.lock()->parent.expired())
	{
		localPosition = value;
	}
	else
	{
		localPosition = GetLocalPositionFromMatrices(transformationMatrix, gameObject.lock()->parent.lock()->GetTransform().lock()->transformationMatrix);
	}

	SetChildrenWorldPositions();
}

void Transform::SetLocalPosition(const Vector3 value)
{
	if (gameObject.lock()->parent.expired())
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
	if (gameObject.lock()->parent.expired())
	{
		localRotation = value;
	}
	else
	{
		localRotation = GetLocalRotationFromWorldRotations(GetRotation(), gameObject.lock()->parent.lock()->GetTransform().lock()->GetRotation());
	}

	SetChildrenWorldPositions();
}

void Transform::SetLocalRotation(const Vector3 value)
{
	if (gameObject.lock()->parent.expired())
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
	if (gameObject.lock()->parent.expired())
	{
		//----- Set new local scale
		localScale = scale / gameObject.lock()->parent.lock()->GetTransform().lock()->scale;

		//----- Set new local rotation
		localRotation = GetLocalRotationFromWorldRotations(GetRotation(), gameObject.lock()->parent.lock()->GetTransform().lock()->GetRotation());

		//----- Set new local position
		localPosition = GetLocalPositionFromMatrices(transformationMatrix, gameObject.lock()->parent.lock()->GetTransform().lock()->transformationMatrix);
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

	int childCount = gameObject.lock()->GetChildrenCount();

	//For each children
	for (int i = 0; i < childCount; i++)
	{
		gameObject.lock()->children[i].lock()->GetTransform().lock()->isTransformationMatrixDirty = true;
		gameObject.lock()->children[i].lock()->GetTransform().lock()->UpdateWorldValues();
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
	if (gameObject.lock()->parent.expired())
		return;

	glm::quat quatParentGlobal = glm::quat(glm::radians(glm::vec3(gameObject.lock()->parent.lock()->GetTransform().lock()->GetRotation().z, gameObject.lock()->parent.lock()->GetTransform().lock()->GetRotation().x, gameObject.lock()->parent.lock()->GetTransform().lock()->GetRotation().y)));
	glm::quat quatChildLocal = glm::quat(glm::radians(glm::vec3(GetLocalRotation().z, GetLocalRotation().x, GetLocalRotation().y)));

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
	if (gameObject.lock()->parent.expired())
		return;

	//Get child local position
	float scaledLocalPos[3] = { (GetLocalPosition().x * gameObject.lock()->parent.lock()->GetTransform().lock()->GetScale().x), -(GetLocalPosition().y * gameObject.lock()->parent.lock()->GetTransform().lock()->GetScale().y), -(GetLocalPosition().z * gameObject.lock()->parent.lock()->GetTransform().lock()->GetScale().z) };

	//Create the matrix which store the new child's world position (wihtout parent's world position added)
	float posAfterRotation[3];
	Math::MultiplyMatrix(scaledLocalPos, gameObject.lock()->parent.lock()->GetTransform().lock()->rotationMatrix, posAfterRotation, 1, 3, 3, 3);

	//Set new child position (with parent's world position added)
	position = Vector3(posAfterRotation[0] + gameObject.lock()->parent.lock()->GetTransform().lock()->GetPosition().x, (-posAfterRotation[1] + gameObject.lock()->parent.lock()->GetTransform().lock()->GetPosition().y), (-posAfterRotation[2] + gameObject.lock()->parent.lock()->GetTransform().lock()->GetPosition().z));
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
	/*if (auto parentGm = gameObject.lock()->parent.lock())
	{
		while (parentGm != nullptr)
		{
			scale = scale * parentGm->GetTransform()->localScale;
			parentGm = parentGm->parent;
		}

		int childCount = gameObject.lock()->GetChildrenCount();
		for (int i = 0; i < childCount; i++)
		{
			GameObject* child = gameObject->children[i];
			child->GetTransform()->UpdateWorldScale();
		}
	}*/
}

Vector3 Transform::GetLocalPositionFromMatrices(glm::mat4 childMatrix, glm::mat4 parentMatrix)
{
	glm::mat4 parentGlobalTransform = parentMatrix;
	glm::mat4 childGlobalTransform = childMatrix;

	glm::mat4 parentGlobalTransformInverse = glm::inverse(parentGlobalTransform);
	glm::mat4 childLocalTransform = parentGlobalTransformInverse * childGlobalTransform;

	glm::vec3 childLocalPosition = glm::vec3(childLocalTransform[3]);

	return Vector3(-childLocalPosition.x, childLocalPosition.y, childLocalPosition.z);
}

Vector3 Transform::GetLocalRotationFromWorldRotations(Vector3 childWorldRotation, Vector3 parentWorldRotation)
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
