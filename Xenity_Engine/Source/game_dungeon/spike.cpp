#include "spike.h"
#include <xenity.h>
#include "player.h"
#include "level_manager.h"

Spike::Spike()
{
	componentName = "Spike";
}

void Spike::Start()
{
}

void Spike::Update()
{
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

ReflectiveData Spike::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, rigidbody, "rigidbody", true);
	return reflectedVariables;
}
