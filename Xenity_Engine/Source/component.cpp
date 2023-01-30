#include "component.h"


Component::Component()
{
}

Component::~Component()
{
}

void Component::SetGameObject(GameObject* gm)
{
	gameObject = gm;
}