#include "transform.h"
#include "gameobject.h"
#include <iostream	>
#include "tools/math.h"
#include <glm/ext/matrix_transform.hpp>
#include <SDL2/SDL_stdinc.h>
#include "tools/math.h"

#pragma region Constructors

Transform::Transform(GameObject* gameObject)
{
	this->gameObject = gameObject;
	UpdateRotationMatrix();
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
	Vector3 direction = Vector3(-rotationMatrix[6], rotationMatrix[7], -rotationMatrix[8]);
	return direction;
}

Vector3 Transform::GetBackward() const
{
	return -GetForward();
}

Vector3 Transform::GetLeft() const
{
	Vector3 direction = Vector3(-rotationMatrix[0], rotationMatrix[1], -rotationMatrix[2]);
	return direction;
}

Vector3 Transform::GetRight() const
{
	return -GetLeft();
}

Vector3 Transform::GetUp() const
{
	Vector3 direction = Vector3(-rotationMatrix[3], rotationMatrix[4], -rotationMatrix[5]);
	return direction;
}

Vector3 Transform::GetDown() const
{
	return -GetUp();
}

void Transform::SetPosition(const Vector3 value)
{
	position = value;
	if (gameObject->parent == nullptr) {
		localPosition = value;
	}
	else
	{
		localPosition = (position - gameObject->parent->transform.position) * localScale;
	}

	if (gameObject != nullptr)
		SetChildrenWorldPositions();
}

void Transform::SetLocalPosition(const Vector3 value)
{
	localPosition = value;
	if (gameObject != nullptr)
		UpdateWorldPosition();
	//gameObject->parent->transform.SetChildrenWorldPositions();
}

void Transform::SetRotation(const Vector3 value)
{
	rotation = value;
	if (gameObject->parent == nullptr) {
		//localRotation = value;
	}
	if (gameObject != nullptr)
		SetChildrenWorldPositions();
}

void Transform::SetLocalRotation(const Vector3 value)
{
	localRotation = value;
	if (gameObject != nullptr)
		SetChildrenWorldPositions();
}

void Transform::SetLocalScale(const Vector3 value)
{
	localScale = value;
	UpdateScale();
	if (gameObject != nullptr)
		SetChildrenWorldPositions();
}

#pragma endregion

void Transform::OnParentChanged() 
{
	//Create the matrix which store the new child's world position (wihtout parent's world position added)
	double modifiedMatrix[9];

	for (int i = 0; i < 9; i++)
	{
		modifiedMatrix[i] = gameObject->parent->transform.rotationMatrix[i];
	}
	//invert angle
	modifiedMatrix[1] = -modifiedMatrix[1];
	modifiedMatrix[2] = -modifiedMatrix[2];
	modifiedMatrix[3] = -modifiedMatrix[3];

	if (gameObject->parent != nullptr) 
	{
		double posAfterRotation[3];
		localScale = scale / gameObject->parent->transform.scale;
		localRotation = rotation - gameObject->parent->transform.rotation;
		//localPosition = (position - gameObject->parent->transform.position) / gameObject->parent->transform.scale;
		Vector3 localPosition2 = (position - gameObject->parent->transform.position) / gameObject->parent->transform.scale;
		double localPos[3] = { localPosition2.x, localPosition2.y , localPosition2.z };
		Math::MultiplyMatrix(localPos, modifiedMatrix, posAfterRotation, 1, 3, 3, 3);
		localPosition = Vector3(posAfterRotation[0], posAfterRotation[1], posAfterRotation[2]);
	}
}

void Transform::UpdateRotationMatrix()
{
	double Deg2Rad = 0.01745329251; //M_PI / 180.0f
	Vector3 radAngles = Vector3();
	radAngles.x = Deg2Rad * -GetRotation().x;
	radAngles.y = Deg2Rad * -GetRotation().y;
	radAngles.z = Deg2Rad * -GetRotation().z;

	double cosX = cos(radAngles.x);
	double sinX = sin(radAngles.x);
	double cosY = cos(radAngles.y);
	double sinY = sin(radAngles.y);
	double cosZ = cos(radAngles.z);
	double sinZ = sin(radAngles.z);

	//Create X, Y and Z matrices
	double rotX[9] = { 1, 0, 0, 0, cosX, -sinX, 0, sinX, cosX };
	double rotY[9] = { cosY, 0,sinY, 0, 1, 0, -sinY, 0, cosY };
	double rotZ[9] = { cosZ, -sinZ, 0, sinZ, cosZ, 0, 0, 0, 1 };

	//Multiply Z with X and with Y (there is a temp matrix because of the multiplication in two steps)
	double tempRotationM[9];
	Math::MultiplyMatrix(rotZ, rotX, tempRotationM, 3, 3, 3, 3);
	Math::MultiplyMatrix(tempRotationM, rotY, rotationMatrix, 3, 3, 3, 3);
}

void Transform::SetChildrenWorldPositions()
{
	UpdateRotationMatrix();
	UpdateTransformationMatrix();

	int childCount = gameObject->children.size();

	//For each children
	for (int i = 0; i < childCount; i++)
	{
		gameObject->children[i]->transform.UpdateWorldPosition();
	}
}

void Transform::UpdateWorldPosition()
{
	if (gameObject->parent == nullptr)
		return;

	//Get child local position
	double scaledLocalPos[3] = { GetLocalPosition().x * gameObject->parent->transform.GetScale().x, GetLocalPosition().y * gameObject->parent->transform.GetScale().y, GetLocalPosition().z * gameObject->parent->transform.GetScale().z };

	//Create the matrix which store the new child's world position (wihtout parent's world position added)
	double posAfterRotation[3];
	Math::MultiplyMatrix(scaledLocalPos, gameObject->parent->transform.rotationMatrix, posAfterRotation, 1, 3, 3, 3);

	//Set new child position (with parent's world position added)
	position = Vector3(posAfterRotation[0] + gameObject->parent->transform.GetPosition().x, posAfterRotation[1] + gameObject->parent->transform.GetPosition().y, posAfterRotation[2] + gameObject->parent->transform.GetPosition().z);

	Vector3 newRotation;
	newRotation = gameObject->parent->transform.GetRotation() + GetLocalRotation();

	rotation = newRotation;

	SetChildrenWorldPositions();
}

void Transform::UpdateLocalScale()
{
	//scale = localScale;

	if (gameObject->parent != nullptr) {
		//std::cout << gameObject->name << ", AVANT localScale: " << localScale.x << " " << localScale.y << " " << localScale.z << " " << std::endl;
		localScale = scale / gameObject->parent->transform.scale;
		//localPosition = localPosition * localScale;
		//std::cout << gameObject->name << ", APRES localScale: " << localScale.x << " " << localScale.y << " " << localScale.z << " " << std::endl;
	}

	/*GameObject* parentGm = gameObject->parent;
	while (parentGm != nullptr)
	{
		//scale = Vector3(0);
		localScale = localScale / parentGm->transform.localScale;
		parentGm = parentGm->parent;
	}*/

	/*int childCount = gameObject->children.size();
	for (int i = 0; i < childCount; i++)
	{
		GameObject* child = gameObject->children[i];
		child->transform.UpdateScale();
	}*/
}

void Transform::UpdateTransformationMatrix() 
{
	transformationMatrix = glm::mat4(1.0f);
	transformationMatrix = glm::translate(transformationMatrix, glm::vec3(position.x, position.y, position.z));
	transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	transformationMatrix = glm::scale(transformationMatrix, glm::vec3(scale.x, scale.y, scale.z));
}

void Transform::UpdateScale()
{
	scale = localScale;

	GameObject* parentGm = gameObject->parent;
	while (parentGm != nullptr)
	{
		scale = scale * parentGm->transform.localScale;
		parentGm = parentGm->parent;
	}

	int childCount = gameObject->children.size();
	for (int i = 0; i < childCount; i++)
	{
		GameObject* child = gameObject->children[i];
		child->transform.UpdateScale();
	}
}