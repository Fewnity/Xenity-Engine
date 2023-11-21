#include "player.h"
#include "../xenity.h"
#include "level_manager.h"

Player::Player()
{
	componentName = "Player";
}

void Player::Start()
{
}

void Player::Update()
{
	if (auto rigidbodyLock = rigidbody.lock())
	{
		// Reset velocity
		rigidbodyLock->velocity.x = 0;
		rigidbodyLock->velocity.z = 0;
		Vector2 input = Vector2(0);

#if defined(_WIN32) || defined(_WIN64)
		if (InputSystem::GetKey(UP))
		{
			input.x = -1;
		}
		else if (InputSystem::GetKey(DOWN))
		{
			input.x = 1;
		}

		if (InputSystem::GetKey(LEFT))
		{
			input.y = -1;
		}
		else if (InputSystem::GetKey(RIGHT))
		{
			input.y = 1;
		}
#else
		input.y = InputSystem::leftJoystick.x;
		input.x = InputSystem::leftJoystick.y;
#endif // defined(_WIN32) || defined(_WIN64)

		rigidbodyLock->velocity.x = input.x * moveSpeed;
		rigidbodyLock->velocity.z = input.y * moveSpeed;

		if (rigidbodyLock->velocity.y >= 0 && oldVel < 0)
		{
			canJump = true;
		}
		oldVel = rigidbodyLock->velocity.y;

		if(canJump)
		{
			if (InputSystem::GetKeyDown(SPACE) || InputSystem::GetKeyDown(CROSS))
			{
				canJump = false;
				rigidbodyLock->velocity.y = jumpForce;
			}
		}


		Vector3 lookDir = Vector3::LookAt(Vector3(0), Vector3(input.x, 0, input.y));
		if (input.x != 0 || input.y != 0)
		{
			lastAngle = lookDir.y;
			GetTransform()->SetRotation(Vector3(0, lookDir.y, 0));
		}
		else
		{
			GetTransform()->SetRotation(Vector3(0, lastAngle, 0));
		}

		if (coinTextRenderer.lock())
		{
			coinTextRenderer.lock()->SetText("Coins: " + std::to_string(coinCount));
		}
	}
}

ReflectiveData Player::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, rigidbody, "rigidbody", true);
	Reflective::AddVariable(reflectedVariables, jumpForce, "jumpForce", true);
	Reflective::AddVariable(reflectedVariables, moveSpeed, "moveSpeed", true);
	Reflective::AddVariable(reflectedVariables, coinCount, "coinCount", true);
	Reflective::AddVariable(reflectedVariables, coinTextRenderer, "coinTextRenderer", true);
	Reflective::AddVariable(reflectedVariables, levelManager, "levelManager", true);
	return reflectedVariables;
}
