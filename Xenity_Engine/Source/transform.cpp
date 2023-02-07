#include "transform.h"

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
}

void Transform::SetLocalPosition(Vector3 value)
{
    localPosition = value;
}

void Transform::SetRotation(Vector3 value)
{
    rotation = value;
}

void Transform::SetLocalRotation(Vector3 value)
{
    localRotation = value;
}

void Transform::SetScale(Vector3 value)
{
    scale = value;
}

void Transform::SetLocalScale(Vector3 value)
{
    localScale = value;
}
