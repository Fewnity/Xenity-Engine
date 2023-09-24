#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <memory>
#include "../component.h"
#include "iDrawableTypes.h"

class API IDrawable : public Component
{
public:
	IDrawable();

	virtual ~IDrawable();

	virtual void Draw() {};
	IDrawableTypes type = Draw_3D;
	virtual int GetDrawPriority() const = 0;
};
