#include "transform.h"
#include "gameobject.h"

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

void Transform::SetPosition(Vector3 value)
{
	position = value;
	if (gameObject != nullptr)
		gameObject->SetChildrenWorldPositions();
}

void Transform::SetLocalPosition(Vector3 value)
{
	localPosition = value;
	if (gameObject != nullptr)
		gameObject->SetChildrenWorldPositions();
}

void Transform::SetRotation(Vector3 value)
{
	rotation = value;
	if (gameObject != nullptr)
		gameObject->SetChildrenWorldPositions();
}

void Transform::SetLocalRotation(Vector3 value)
{
	localRotation = value;
	if (gameObject != nullptr)
		gameObject->SetChildrenWorldPositions();
}

/*void Transform::SetScale(Vector3 value)
{
	scale = value;
	if (gameObject != nullptr)
		gameObject->SetChildrenWorldPositions();
}*/

void Transform::SetLocalScale(Vector3 value)
{
	localScale = value;
	UpdateScale();
	if (gameObject != nullptr)
		gameObject->SetChildrenWorldPositions();
}

void Transform::UpdateLocalScale()
{
	//scale = localScale;

	GameObject* parentGm = gameObject->parent;
	while (parentGm != nullptr)
	{
		//scale = Vector3(0);
		localScale = localScale / parentGm->transform.localScale;
		parentGm = parentGm->parent;
	}

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
		//scale = Vector3(0);
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