#include "enemy.h"
#include <xenity.h>
#include "player.h"
#include "level_manager.h"
#if defined(EDITOR)
#include <xenity_editor.h>
#endif

Enemy::Enemy()
{
	componentName = "Enemy";
}

void Enemy::Start()
{
}

void Enemy::Update()
{
	auto transform = GetTransform();

	float distance = 0;
	if (currentWaypoint == 0)
	{
		distance = Vector3::Distance(transform->GetPosition(), waypoint0.lock()->GetTransform()->GetPosition());
	}
	else
	{
		distance = Vector3::Distance(transform->GetPosition(), waypoint1.lock()->GetTransform()->GetPosition());
	}

	if (waitCurrentCooldown > 0)
	{
		waitCurrentCooldown -= Time::GetDeltaTime();
	}

	if (distance <= 0.1f)
	{
		currentWaypoint++;
		currentWaypoint = currentWaypoint % 2;
		waitCurrentCooldown = waitCooldown;
	}
	else if (waitCurrentCooldown <= 0)
	{
		Vector3 newPosition;
		Vector3 lookRotation;
		float speed = 1 / distance;
		if (currentWaypoint == 0)
		{
			newPosition = Vector3::Lerp(transform->GetPosition(), waypoint0.lock()->GetTransform()->GetPosition(), Time::GetDeltaTime() * speed);
			lookRotation = Vector3::LookAt(transform->GetPosition(), waypoint0.lock()->GetTransform()->GetPosition());
		}
		else
		{
			newPosition = Vector3::Lerp(transform->GetPosition(), waypoint1.lock()->GetTransform()->GetPosition(), Time::GetDeltaTime() * speed);
			lookRotation = Vector3::LookAt(transform->GetPosition(), waypoint1.lock()->GetTransform()->GetPosition());
		}
		transform->SetPosition(newPosition);
		transform->SetRotation(lookRotation);
	}

	if (auto rigidbodyLock = rigidbody.lock())
	{
		size_t inTriggerCount = rigidbodyLock->inTrigger.size();
		for (size_t i = 0; i < inTriggerCount; i++)
		{
			std::weak_ptr<Player> player = rigidbodyLock->inTrigger[i].lock()->GetGameObject()->GetComponent<Player>();
			if (auto playerLock = player.lock())
			{
				playerLock->levelManager.lock()->RestartLevel();
			}
		}
	}
}

ReflectiveData Enemy::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, rigidbody, "rigidbody", true);
	Reflective::AddVariable(reflectedVariables, waypoint0, "waypoint0", true);
	Reflective::AddVariable(reflectedVariables, waypoint1, "waypoint1", true);
	Reflective::AddVariable(reflectedVariables, currentWaypoint, "currentWaypoint", true);
	Reflective::AddVariable(reflectedVariables, waitCooldown, "waitCooldown", true);
	Reflective::AddVariable(reflectedVariables, waitCurrentCooldown, "waitCurrentCooldown", false);
	return reflectedVariables;
}

void Enemy::OnDrawGizmos()
{
#if defined(EDITOR)
	Color lineColor = Color::CreateFromRGBAFloat(0, 1, 0, 1);
	Gizmo::SetColor(lineColor);

	Engine::GetRenderer().SetCameraPosition(Graphics::usedCamera.lock());

	Vector3 pos0 = waypoint0.lock()->GetTransform()->GetPosition();
	pos0.x = -pos0.x;
	Vector3 pos1 = waypoint1.lock()->GetTransform()->GetPosition();
	pos1.x = -pos1.x;

	// Bottom
	Gizmo::DrawLine(pos0, pos1);
#endif
}
