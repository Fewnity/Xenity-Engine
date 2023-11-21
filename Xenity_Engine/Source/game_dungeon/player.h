#pragma once

#include "../engine/monobehaviour.h"
#include "../engine/component.h"
#include "../engine/physics/rigidbody.h"

class TextRendererCanvas;
class LevelManager;

class Player : public MonoBehaviour
{
public:
	Player();
	void Start() override;
	void Update() override;
	ReflectiveData GetReflectiveData() override;
	std::weak_ptr<RigidBody> rigidbody;
	float jumpForce = 4;
	float moveSpeed = 3;
	int coinCount = 0;
	std::weak_ptr<TextRendererCanvas> coinTextRenderer;
	std::weak_ptr<LevelManager> levelManager;
	float lastAngle = -90;

	bool canJump = true;
	float oldVel = 0;
private:
};
