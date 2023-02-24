#pragma once

#include "vectors/vector3.h"
#include <glm/glm.hpp>

class GameObject;

class Transform
{

public:
	Transform() = delete;
	Transform(GameObject* gameObject);

	Vector3 GetPosition() const;
	Vector3 GetLocalPosition() const;
	Vector3 GetRotation() const;//Euler angle
	Vector3 GetLocalRotation() const;//Euler angle
	Vector3 GetScale() const;
	Vector3 GetLocalScale() const;

	Vector3 GetForward() const;
	Vector3 GetLeft() const;
	Vector3 GetRight() const;
	Vector3 GetUp() const;
	Vector3 GetDown() const;
	Vector3 GetBackward() const;

	void SetPosition(const Vector3 value);
	void SetLocalPosition(const Vector3 value);
	void SetRotation(const Vector3 value);//Euler angle
	void SetLocalRotation(const Vector3 value);//Euler angle
	void SetLocalScale(const Vector3 value);
	void UpdateWorldScale();
	void SetChildrenWorldPositions();
	void OnParentChanged();
	glm::mat4 transformationMatrix;

private:
	void UpdateTransformationMatrix();
	void UpdateWorldValues();
	void UpdateWorldPosition();
	void UpdateWorldRotation();
	Vector3 position = Vector3(0);
	Vector3 localPosition = Vector3(0);
	Vector3 rotation = Vector3(0);//Euler angle
	Vector3 localRotation = Vector3(0);//Euler angle
	Vector3 scale = Vector3(1);
	Vector3 localScale = Vector3(1);
	GameObject* gameObject = nullptr;
	double rotationMatrix[9] = { 0 };
};

