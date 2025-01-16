// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <glm/mat4x4.hpp>
#include <memory>

#include <engine/api.h>
#include <engine/event_system/event_system.h>
#include <engine/vectors/vector3.h>
#include <engine/vectors/quaternion.h>
#include <engine/constants.h>

class GameObject;

class API Transform : public Reflective, public std::enable_shared_from_this<Transform>
{

public:
	Transform() = delete;
	explicit Transform(const std::shared_ptr<GameObject>& gameObject);
	virtual ~Transform() = default;

	/**
	* @brief Get position
	*/
	inline const Vector3& GetPosition() const
	{
		return m_position;
	}

	/**
	* @brief Get local position
	*/
	inline const Vector3& GetLocalPosition() const
	{
		return m_localPosition;
	}

	/**
	* @brief Get rotation (in degree)
	*/
	inline const Vector3& GetEulerAngles() const
	{
		return m_rotation;
	}

	/**
	* @brief Get local rotation (in degree)
	*/
	inline const Vector3& GetLocalEulerAngles() const
	{
		return m_localRotation;
	}

	/**
	* @brief Get rotation
	*/
	inline const Quaternion& GetRotation() const
	{
		return m_rotationQuaternion;
	}

	/**
	* @brief Get local rotation
	*/
	inline const Quaternion& GetLocalRotation() const
	{
		return m_localRotationQuaternion;
	}

	/**
	* @brief Get scale
	*/
	inline const Vector3& GetScale() const
	{
		return m_scale;
	}

	/**
	* @brief Get local scale
	*/
	inline const Vector3& GetLocalScale() const
	{
		return m_localScale;
	}

	/**
	* @brief Get forward direction
	*/
	inline Vector3 GetForward() const
	{
		const Vector3 direction = Vector3(-rotationMatrix[6], rotationMatrix[7], rotationMatrix[8]);
		return direction;
	}

	/**
	* @brief Get backward direction
	*/
	inline Vector3 GetBackward() const
	{
		return -GetForward();
	}

	/**
	* @brief Get left direction
	*/
	inline Vector3 GetLeft() const
	{
		return -GetRight();
	}

	/**
	* @brief Get right direction
	*/
	inline Vector3 GetRight() const
	{
		const Vector3 direction = Vector3(rotationMatrix[0], -rotationMatrix[1], -rotationMatrix[2]);
		return direction;
	}

	/**
	* @brief Get up direction
	*/
	inline Vector3 GetUp() const
	{
		const Vector3 direction = Vector3(-rotationMatrix[3], rotationMatrix[4], rotationMatrix[5]);
		return direction;
	}

	/**
	* @brief Get down direction
	*/
	inline Vector3 GetDown() const
	{
		return -GetUp();
	}

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
	* @brief Set rotation
	* @param value Rotation
	*/
	void SetRotation(const Quaternion& value);

	/**
	* @brief Set local rotation
	* @param value Local rotation
	*/
	void SetLocalRotation(const Quaternion& value);

	/**
	* @brief Set local scale
	* @param value Local scale
	*/
	void SetLocalScale(const Vector3& value);

	inline const glm::mat4& GetTransformationMatrix() const
	{
		return transformationMatrix;
	}

	inline const glm::mat3& GetInverseNormalMatrix()
	{
		if constexpr (!s_UseOpenGLFixedFunctions)
		{
			if (m_isNormalMatrixDirty)
			{
				normalMatrix = glm::transpose(glm::inverse(glm::mat3(transformationMatrix)));
				m_isNormalMatrixDirty = false;
			}
		}
		return normalMatrix;
	}

	const glm::mat4& GetMVPMatrix(size_t currentFrame);

	/**
	* @brief Get GameObject
	*/
	inline std::shared_ptr<GameObject> GetGameObject() const
	{
		return m_gameObject.lock();
	}

	/**
	* Get the event that is called when the transform is updated (new position, or new rotation or new scale)
	*/
	Event<>& GetOnTransformUpdated()
	{
		return m_onTransformUpdated;
	}

	/**
	* Get the event that is called when the transform is scamled
	*/
	Event<>& GetOnTransformScaled()
	{
		return m_onTransformScaled;
	}


private:
	glm::mat4 transformationMatrix;
	friend class RigidBody;

	Quaternion m_rotationQuaternion = Quaternion::Identity();
	Quaternion m_localRotationQuaternion = Quaternion::Identity();
	Event<> m_onTransformUpdated;
	Event<> m_onTransformScaled;

	ReflectiveData GetReflectiveData() override;

	friend class InspectorSetTransformDataCommand;
	friend class InspectorDeleteGameObjectCommand;
	friend class GameObject;
	friend class SceneManager;
	friend class InspectorMenu;

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

	void UpdateLocalRotation();

	Vector3 m_position = Vector3(0);
	Vector3 m_localPosition = Vector3(0);
	Vector3 m_rotation = Vector3(0);//Euler angle
	Vector3 m_localRotation = Vector3(0);//Euler angle

	Vector3 m_scale = Vector3(1);
	Vector3 m_localScale = Vector3(1);

	std::weak_ptr<GameObject> m_gameObject;
	glm::mat4 mvpMatrix;
	float rotationMatrix[9] = { 0,0,0,0,0,0,0,0,0 };
	glm::mat3 normalMatrix;

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
	size_t lastMVPFrame = 0;
	bool m_isNormalMatrixDirty = true;
public:
	// [Internal]
	bool m_isTransformationMatrixDirty = true;
};

