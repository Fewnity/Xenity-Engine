#pragma once

#include "vectors/vector3.h"
#include <glm/glm.hpp>

class GameObject;

class Transform
{

public:
	Transform() = delete;
	Transform(GameObject* gameObject);

	Vector3 GetPosition();
	Vector3 GetLocalPosition();
	Vector3 GetRotation();//Euler angle
	Vector3 GetLocalRotation();//Euler angle
	Vector3 GetScale();
	Vector3 GetLocalScale();

	Vector3 GetForward();

	void SetPosition(Vector3 value);
	void SetLocalPosition(Vector3 value);
	void SetRotation(Vector3 value);//Euler angle
	void SetLocalRotation(Vector3 value);//Euler angle
	void SetLocalScale(Vector3 value);
	void UpdateScale();
	void UpdateLocalScale();
	void SetChildrenWorldPositions();
	void OnParentChanged();
	glm::mat4 transformationMatrix;

private:
	void UpdateTransformationMatrix();
	void UpdateWorldPosition();
	void UpdateRotationMatrix();
	Vector3 position = Vector3(0);
	Vector3 localPosition = Vector3(0);
	Vector3 rotation = Vector3(0);//Euler angle
	Vector3 localRotation = Vector3(0);//Euler angle
	Vector3 scale = Vector3(1);
	Vector3 localScale = Vector3(1);
	GameObject* gameObject = nullptr;
	double rotationMatrix[9] = { 0 };
};

