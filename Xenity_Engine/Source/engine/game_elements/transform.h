#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include "../vectors/vector3.h"
#include <glm/glm.hpp>
#include <memory>

class GameObject;

class API Transform : public Reflection
{

public:
	Transform() = delete;
	Transform(std::weak_ptr<GameObject> gameObject);
	//void SetReflection();

	std::unordered_map<std::string, Variable> GetReflection();
	

	virtual ~Transform() = default;

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
	void SetChildrenWorldPositions();
	void OnParentChanged();
	glm::mat4 transformationMatrix;
	bool movedLastFrame = true;

	std::shared_ptr<GameObject> GetGameObject()
	{
		return gameObject.lock();
	}

	void UpdateWorldValues();
	bool isTransformationMatrixDirty = true;

private:
	void UpdateTransformationMatrix();
	void UpdateWorldPosition();
	void UpdateWorldRotation();
	void UpdateWorldScale();

	Vector3 position = Vector3(0);
	Vector3 localPosition = Vector3(0);
	Vector3 rotation = Vector3(0);//Euler angle
	Vector3 localRotation = Vector3(0);//Euler angle
	Vector3 scale = Vector3(1);
	Vector3 localScale = Vector3(1);
	std::weak_ptr<GameObject> gameObject;
	float rotationMatrix[9] = { 0,0,0,0,0,0,0,0,0 };
	Vector3 GetLocalPositionFromMatrices(glm::mat4 childMatrix, glm::mat4 parentMatrix);
	Vector3 GetLocalRotationFromWorldRotations(Vector3 childWorldRotation, Vector3 parentWorldRotation);

};

