#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include "component.h"

class API MonoBehaviour : public Component
{
public:
	MonoBehaviour();
};