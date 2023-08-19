#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include "component.h"

/**
* Add this to a class to act like a component
*/
class API MonoBehaviour : public Component
{
public:
	MonoBehaviour();
};