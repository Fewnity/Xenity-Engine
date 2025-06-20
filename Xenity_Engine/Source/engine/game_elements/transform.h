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
#include <engine/math/vector3.h>
#include <engine/math/quaternion.h>
#include <engine/constants.h>

class GameObject;

/**
* @brief Class representing a 3D transformation (position, rotation, scale) of a GameObject
*/
class API Transform : public Reflective, public std::enable_shared_from_this<Transform>
{

public:
	Transform() = delete;
	explicit Transform(const std::shared_ptr<GameObject>& gameObject);
	virtual ~Transform() = default;

	/**
	* @brief Get position
	*/
	[[nodiscard]] const Vector3& GetPosition() const
	{
		return m_position;
	}

	/**
	* @brief Set position
	* @param value Position
	*/
	void SetPosition(const Vector3& position);

	/**
	* @brief Get local position
	*/
	[[nodiscard]] const Vector3& GetLocalPosition() const
	{
		return m_localPosition;
	}

	/**
	* @brief Set local position
	* @param value Local position
	*/
	void SetLocalPosition(const Vector3& position);

	/**
	* @brief Get rotation (in degree)
	*/
	[[nodiscard]] const Vector3& GetEulerAngles() const
	{
		return m_rotation;
	}

	/**
	* @brief Get local rotation (in degree)
	*/
	[[nodiscard]] const Vector3& GetLocalEulerAngles() const
	{
		return m_localRotation;
	}

	/**
	* @brief Get rotation
	*/
	[[nodiscard]] const Quaternion& GetRotation() const
	{
		return m_rotationQuaternion;
	}

	/**
	* @brief Set rotation in degree
	* @param value Rotation in degree
	*/
	void SetEulerAngles(const Vector3& rotation);

	/**
	* @brief Set rotation
	* @param value Rotation
	*/
	void SetRotation(const Quaternion& rotation);

	/**
	* @brief Get local rotation
	*/
	[[nodiscard]] const Quaternion& GetLocalRotation() const
	{
		return m_localRotationQuaternion;
	}

	/**
	* @brief Set local rotation in degree
	* @param value Local rotation in degree
	*/
	void SetLocalEulerAngles(const Vector3& rotation);

	/**
	* @brief Set local rotation
	* @param value Local rotation
	*/
	void SetLocalRotation(const Quaternion& rotation);

	/**
	* @brief Get scale
	*/
	[[nodiscard]] const Vector3& GetScale() const
	{
		return m_scale;
	}

	/**
	* @brief Get local scale
	*/
	[[nodiscard]] const Vector3& GetLocalScale() const
	{
		return m_localScale;
	}

	/**
	* @brief Set local scale
	* @param value Local scale
	*/
	void SetLocalScale(const Vector3& scale);

	/**
	* @brief Get forward direction
	*/
	[[nodiscard]] Vector3 GetForward() const
	{
		const Vector3 direction = Vector3(-rotationMatrix[6], rotationMatrix[7], rotationMatrix[8]);
		return direction;
	}

	/**
	* @brief Get backward direction
	*/
	[[nodiscard]] Vector3 GetBackward() const
	{
		return -GetForward();
	}

	/**
	* @brief Get left direction
	*/
	[[nodiscard]] Vector3 GetLeft() const
	{
		return -GetRight();
	}

	/**
	* @brief Get right direction
	*/
	[[nodiscard]] Vector3 GetRight() const
	{
		const Vector3 direction = Vector3(rotationMatrix[0], -rotationMatrix[1], -rotationMatrix[2]);
		return direction;
	}

	/**
	* @brief Get up direction
	*/
	[[nodiscard]] Vector3 GetUp() const
	{
		const Vector3 direction = Vector3(-rotationMatrix[3], rotationMatrix[4], rotationMatrix[5]);
		return direction;
	}

	/**
	* @brief Get down direction
	*/
	[[nodiscard]] Vector3 GetDown() const
	{
		return -GetUp();
	}

	/**
	* @brief Set transformation matrix
	*/
	[[nodiscard]] const glm::mat4& GetTransformationMatrix() const
	{
		return transformationMatrix;
	}

	/**
	* @brief Set transformation matrix
	*/
	void SetTransformationMatrix(const glm::mat4& matrix);

	/**
	* @brief Get GameObject
	*/
	[[nodiscard]] std::shared_ptr<GameObject> GetGameObject() const
	{
		return m_gameObject.lock();
	}

	/**
	* Get the event that is called when the transform is updated (new position, or new rotation or new scale)
	*/
	[[nodiscard]] Event<>& GetOnTransformUpdated()
	{
		return m_onTransformUpdated;
	}

	/**
	* Get the event that is called when the transform is scaled
	*/
	[[nodiscard]] Event<>& GetOnTransformScaled()
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

	[[nodiscard]] ReflectiveData GetReflectiveData() override;

	friend class InspectorSetTransformDataCommand;
	friend class InspectorDeleteGameObjectCommand;
	friend class GameObject;
	friend class SceneManager;
	friend class InspectorMenu;
	friend class MeshManager;
	friend class SpriteManager;

	/**
	* @brief [Internal] Update children world positions
	*/
	void SetChildrenWorldPositions();

	[[nodiscard]] const glm::mat4& GetMVPMatrix(size_t currentFrame);

	[[nodiscard]] const glm::mat3& GetInverseNormalMatrix()
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
	[[nodiscard]] Vector3 GetLocalPositionFromMatrices(const glm::mat4& childMatrix, const glm::mat4& parentMatrix) const;

	/**
	* @brief Get localRotation from matrices
	* @param childMatrix The child matrix
	* @param parentMatrix The parent matrix
	* @return The local rotation
	*/
	[[nodiscard]] Vector3 GetLocalRotationFromWorldRotations(const Vector3& childWorldRotation, const Vector3& parentWorldRotation) const;
	size_t lastMVPFrame = 0;
	bool m_isNormalMatrixDirty = true;
public:
	// [Internal]
	bool m_isTransformationMatrixDirty = true;
};

