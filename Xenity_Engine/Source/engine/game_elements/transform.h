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

class API Transform : public Reflection, public std::enable_shared_from_this<Transform>
{

public:
	Transform() = delete;
	Transform(const std::weak_ptr<GameObject>& gameObject);
	//void SetReflection();

	std::unordered_map<std::string, ReflectionEntry> GetReflection() override;


	virtual ~Transform() = default;

	/**
	* Get position
	*/
	Vector3 GetPosition() const;

	/**
	* Get local position
	*/
	Vector3 GetLocalPosition() const;

	/**
	* Get rotation (in degree)
	*/
	Vector3 GetRotation() const;

	/**
	* Get locla rotation (in degree)
	*/
	Vector3 GetLocalRotation() const;

	/**
	* Get scale
	*/
	Vector3 GetScale() const;

	/**
	* Get local scale
	*/
	Vector3 GetLocalScale() const;

	/**
	* Get forward direction
	*/
	Vector3 GetForward() const;

	/**
	* Get left direction
	*/
	Vector3 GetLeft() const;

	/**
	* Get right direction
	*/
	Vector3 GetRight() const;

	/**
	* Get up direction
	*/
	Vector3 GetUp() const;

	/**
	* Get down direction
	*/
	Vector3 GetDown() const;

	/**
	* Get backward direction
	*/
	Vector3 GetBackward() const;

	/**
	* Set position
	* @param value Position
	*/
	void SetPosition(const Vector3& value);

	/**
	* Set local position
	* @param value Local position
	*/
	void SetLocalPosition(const Vector3& value);

	/**
	* Set rotation (in degree)
	* @param value Rotation (in degree)
	*/
	void SetRotation(const Vector3& value);//Euler angle

	/**
	* Set local rotation (in degree)
	* @param value Local rotation (in degree)
	*/
	void SetLocalRotation(const Vector3& value);//Euler angle

	/**
	* Set local scale
	* @param value Local scale
	*/
	void SetLocalScale(const Vector3& value);

	/**
	* [Internal] Update children world positions
	*/
	void SetChildrenWorldPositions();

	/**
	* Function called when the parent changed
	*/
	void OnParentChanged();

	glm::mat4 transformationMatrix;
	bool movedLastFrame = true;

	/**
	* Get GameObject
	*/
	std::shared_ptr<GameObject> GetGameObject()
	{
		return gameObject.lock();
	}

	/**
	* [Internal] Update world values
	*/
	void UpdateWorldValues();
	bool isTransformationMatrixDirty = true;

private:

	/**
	* Update transformation matrix
	*/
	void UpdateTransformationMatrix();

	/**
	* Update world position
	*/
	void UpdateWorldPosition();

	/**
	* Update world rotation
	*/
	void UpdateWorldRotation();

	/**
	* Update world scale
	*/
	void UpdateWorldScale();

	Vector3 position = Vector3(0);
	Vector3 localPosition = Vector3(0);
	Vector3 rotation = Vector3(0);//Euler angle
	Vector3 localRotation = Vector3(0);//Euler angle
	Vector3 scale = Vector3(1);
	Vector3 localScale = Vector3(1);
	std::weak_ptr<GameObject> gameObject;
	float rotationMatrix[9] = { 0,0,0,0,0,0,0,0,0 };

	/**
	* Get localPosition from matrices
	*/
	Vector3 GetLocalPositionFromMatrices(const glm::mat4& childMatrix, const glm::mat4& parentMatrix);

	/**
	*
	*/
	Vector3 GetLocalRotationFromWorldRotations(const Vector3& childWorldRotation, const Vector3& parentWorldRotation);

};

