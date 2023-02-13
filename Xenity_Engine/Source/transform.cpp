#include "transform.h"
#include "gameobject.h"
#include <iostream>

/**
* TODO : Move in a math class
*/
void MultiplyMatrix(const double* A, const double* B, double* result, int rA, int cA, int rB, int cB)
{
	if (cA != rB)
	{
		return;
	}

	double temp = 0;

	for (int i = 0; i < rA; i++)
	{
		for (int j = 0; j < cB; j++)
		{
			temp = 0;
			for (int k = 0; k < cA; k++)
			{
				temp += A[i * cA + k] * B[k * cB + j];
			}
			result[i * cB + j] = temp;
		}
	}
}

Transform::Transform(GameObject* gameObject)
{
	this->gameObject = gameObject;
}

Vector3 Transform::GetPosition()
{
	return position;
}

Vector3 Transform::GetLocalPosition()
{
	return localPosition;
}

Vector3 Transform::GetRotation()
{
	return rotation;
}

Vector3 Transform::GetLocalRotation()
{
	return localRotation;
}

Vector3 Transform::GetScale()
{
	return scale;
}

Vector3 Transform::GetLocalScale()
{
	return localScale;
}

void Transform::OnParentChanged() 
{
	double Deg2Rad = 0.01745329251; //M_PI / 180.0f
	Vector3 radAngles = Vector3();
	radAngles.x = Deg2Rad * gameObject->parent->transform.GetRotation().x;
	radAngles.y = Deg2Rad * gameObject->parent->transform.GetRotation().y;
	radAngles.z = Deg2Rad * gameObject->parent->transform.GetRotation().z;

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
	double rotationM[9];
	MultiplyMatrix(rotZ, rotX, tempRotationM, 3, 3, 3, 3);
	MultiplyMatrix(tempRotationM, rotY, rotationM, 3, 3, 3, 3);

	//Create the matrix which store the new child's world position (wihtout parent's world position added)
	double posAfterRotation[3];

	std::cout << gameObject->name<< " S:" << localScale.x << std::endl;
	std::cout << GetPosition().x << " " << GetPosition().y << " " << GetPosition().z << " " << std::endl;
	std::cout << GetLocalPosition().x << " " << GetLocalPosition().y << " " << GetLocalPosition().z << " " << std::endl;
	std::cout << gameObject->parent->transform.position.x << " " << gameObject->parent->transform.position.y << " " << gameObject->parent->transform.position.z << " " << std::endl;

	if (gameObject->parent != nullptr) {
		localScale = scale / gameObject->parent->transform.scale;
		localRotation = rotation - gameObject->parent->transform.rotation;
		//localPosition = (position - gameObject->parent->transform.position) / gameObject->parent->transform.scale;
		Vector3 localPosition2 = (position - gameObject->parent->transform.position) / gameObject->parent->transform.scale;
		double localPos[3] = { localPosition2.x, localPosition2.y , localPosition2.z };
		MultiplyMatrix(localPos, rotationM, posAfterRotation, 1, 3, 3, 3);
		localPosition = Vector3(posAfterRotation[0], posAfterRotation[1], posAfterRotation[2]);
	}

	std::cout << gameObject->name << " S:" << localScale.x << std::endl;
	std::cout << GetPosition().x << " " << GetPosition().y << " " << GetPosition().z << " " << std::endl;
	std::cout << GetLocalPosition().x << " " << GetLocalPosition().y << " " << GetLocalPosition().z << " " << std::endl;
	std::cout << gameObject->parent->transform.position.x << " " << gameObject->parent->transform.position.y << " " << gameObject->parent->transform.position.z << " " << std::endl;
	std::cout << "--------------------------" << std::endl;
}

void Transform::SetPosition(Vector3 value)
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

void Transform::SetLocalPosition(Vector3 value)
{
	localPosition = value;
	if (gameObject != nullptr)
		SetChildrenWorldPositions();
}

void Transform::SetRotation(Vector3 value)
{
	rotation = value;
	if (gameObject->parent == nullptr) {
		//localRotation = value;
	}
	if (gameObject != nullptr)
		SetChildrenWorldPositions();
}

void Transform::SetLocalRotation(Vector3 value)
{
	localRotation = value;
	if (gameObject != nullptr)
		SetChildrenWorldPositions();
}

void Transform::SetLocalScale(Vector3 value)
{
	localScale = value;
	UpdateScale();
	if (gameObject != nullptr)
		SetChildrenWorldPositions();
}

void Transform::SetChildrenWorldPositions()
{
	double Deg2Rad = 0.01745329251; //M_PI / 180.0f
	int childCount = gameObject->children.size();
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
	double rotationM[9];
	//MultiplyMatrix(rotZ, rotX, tempRotationM, 3, 3, 3, 3);
	//MultiplyMatrix(tempRotationM, rotY, rotationM, 3, 3, 3, 3);
	MultiplyMatrix(rotX, rotY, tempRotationM, 3, 3, 3, 3);
	MultiplyMatrix(tempRotationM, rotZ, rotationM, 3, 3, 3, 3);

	//For each children
	for (int i = 0; i < childCount; i++)
	{
		GameObject* child = gameObject->children[i];

		//Get child local position
		double localPos[3] = { child->transform.GetLocalPosition().x * GetScale().x, child->transform.GetLocalPosition().y * GetScale().y, child->transform.GetLocalPosition().z * GetScale().z };

		//Create the matrix which store the new child's world position (wihtout parent's world position added)
		double posAfterRotation[3];
		MultiplyMatrix(localPos, rotationM, posAfterRotation, 1, 3, 3, 3);
		//Set new child position (with parent's world position added)
		child->transform.position = Vector3(posAfterRotation[0] + GetPosition().x, posAfterRotation[1] + GetPosition().y, posAfterRotation[2] + GetPosition().z);

		Vector3 newRotation;
		newRotation = GetRotation() + child->transform.GetLocalRotation();
		 
		//child->transform.SetRotation(newRotation);
		child->transform.rotation = newRotation;

		child->transform.SetChildrenWorldPositions();

		//Update other child's children positions
	}
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