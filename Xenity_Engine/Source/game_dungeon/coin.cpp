#include "coin.h"
#include "../xenity.h"
#include "player.h"

Coin::Coin()
{
	componentName = "Coin";
}

void Coin::Start()
{
}

void Coin::Update()
{
	auto transform = GetTransform();
	Vector3 rot = transform->GetRotation();
	rot.y += rotateSpeed * Time::GetDeltaTime();
	transform->SetRotation(rot);

	if (auto rigidbodyLock = rigidbody.lock())
	{
		size_t inTriggerCount = rigidbodyLock->inTrigger.size();
		for (size_t i = 0; i < inTriggerCount; i++)
		{
			std::weak_ptr<Player> player = rigidbodyLock->inTrigger[i].lock()->GetGameObject()->GetComponent<Player>();
			if (auto playerLock = player.lock())
			{
				playerLock->coinCount++;
				GetGameObject()->SetActive(false);
			}
		}
	}
}

ReflectiveData Coin::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, rigidbody, "rigidbody", true);
	Reflective::AddVariable(reflectedVariables, rotateSpeed, "rotateSpeed", true);
	return reflectedVariables;
}
