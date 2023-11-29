#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once
#include <engine/vectors/vector2_int.h>

class API TouchRaw
{
public:
	Vector2Int position = Vector2Int(0);
	int force = 0;
	int fingerId = 0;
	int screenIndex = 0;
};