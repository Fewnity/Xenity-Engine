#include "chest.h"
#include <xenity.h>
#include "player.h"
#include "level_manager.h"

Chest::Chest()
{
	componentName = "Chest";
}

void Chest::Start()
{
}

void Chest::Update()
{
	if (auto rigidbodyLock = rigidbody.lock())
	{
		size_t inTriggerCount = rigidbodyLock->inTrigger.size();
		for (size_t i = 0; i < inTriggerCount; i++)
		{
			std::weak_ptr<Player> player = rigidbodyLock->inTrigger[i].lock()->GetGameObject()->GetComponent<Player>();
			if (auto playerLock = player.lock())
			{
				playerLock->levelManager.lock()->LoadNextLevel();
			}
		}
	}
}

ReflectiveData Chest::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, rigidbody, "rigidbody", true);
	return reflectedVariables;
}
