#include "component.h"


Component::Component()
{
}

Component::~Component()
{
}

void Component::Update()
{
}

void Component::SetParent(GameObject* gm)
{
	gameObject = gm;
	gameObject->AddComponent(this);
}