#pragma once
#include <engine/api.h>

#include "component.h"

/**
* Add this to a class to act like a component
*/
class API MonoBehaviour : public Component
{
public:
	MonoBehaviour();
};