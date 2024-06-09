// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

#include <engine/vectors/vector3.h>
#include <glm/glm.hpp>
#include <memory>

class GameObject;

class API Transform : public Reflective, public std::enable_shared_from_this<Transform>
{

public:
	Transform() = delete;
	explicit Transform(const std::shared_ptr<GameObject>& gameObject);
	virtual ~Transform() = default;

	ReflectiveData GetReflectiveData() override;

	/**
	* @brief Get position
	*/
	const Vector3& GetPosition() const;

	/**
	* @brief Get local position
	*/
	const Vector3& GetLocalPosition() const;

	/**
	* @brief Get rotation (in degree)
	*/
	const Vector3& GetRotation() const;

	/**
	* @brief Get local rotation (in degree)
	*/
	const Vector3& GetLocalRotation() const;

	/**
	* @brief Get scale
	*/
	const Vector3& GetScale() const;

	/**
	* @brief Get local scale
	*/
	const Vector3& GetLocalScale() const;

	/**
	* @brief Get forward direction
	*/
	Vector3 GetForward() const;

	/**
	* @brief Get left direction
	*/
	Vector3 GetLeft() const;

	/**
	* @brief Get right direction
	*/
	Vector3 GetRight() const;

	/**
	* @brief Get up direction
	*/
	Vector3 GetUp() const;

	/**
	* @brief Get down direction
	*/
	Vector3 GetDown() const;

	/**
	* @brief Get backward direction
	*/
	Vector3 GetBackward() const;

	/**
	* @brief Set position
	* @param value Position
	*/
	void SetPosition(const Vector3& value);

	/**
	* @brief Set local position
	* @param value Local position
	*/
	void SetLocalPosition(const Vector3& value);

	/**
	* @brief Set rotation (in degree)
	* @param value Rotation (in degree)
	*/
	void SetRotation(const Vector3& value);//Euler angle

	/**
	* @brief Set local rotation (in degree)
	* @param value Local rotation (in degree)
	*/
	void SetLocalRotation(const Vector3& value);//Euler angle

	/**
	* @brief Set local scale
	* @param value Local scale
	*/
	void SetLocalScale(const Vector3& value);

	inline const glm::mat4& GetTransformationMatrix() 
	{
		return transformationMatrix;
	}

	/**
	* @brief Get GameObject
	*/
	inline std::shared_ptr<GameObject> GetGameObject() const
	{
		return gameObject.lock();
	}

private:

	friend class InspectorSetTransformDataCommand;
	friend class InspectorDeleteGameObjectCommand;
	friend class GameObject;
	friend class SceneManager;

	/**
	* @brief [Internal] Update children world positions
	*/
	void SetChildrenWorldPositions();

	/**
	* @brief Function called when the parent changed
	*/
	void OnParentChanged();

	/**
	* @brief [Internal] Update world values
	*/
	void UpdateWorldValues();

	glm::mat4 transformationMatrix;

	/**
	* @brief Update transformation matrix
	*/
	void UpdateTransformationMatrix();

	/**
	* @brief Update world position
	*/
	void UpdateWorldPosition();

	/**
	* @brief Update world rotation
	*/
	void UpdateWorldRotation();

	/**
	* @brief Update world scale
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
	* @brief Get localPosition from matrices
	* @param childMatrix The child matrix
	* @param parentMatrix The parent matrix
	* @return The local position
	*/
	Vector3 GetLocalPositionFromMatrices(const glm::mat4& childMatrix, const glm::mat4& parentMatrix) const;

	/**
	* @brief Get localRotation from matrices
	* @param childMatrix The child matrix
	* @param parentMatrix The parent matrix
	* @return The local rotation
	*/
	Vector3 GetLocalRotationFromWorldRotations(const Vector3& childWorldRotation, const Vector3& parentWorldRotation) const;

public:
	// [Internal]
	bool isTransformationMatrixDirty = true;
};

